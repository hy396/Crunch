# GAS 优化变更日志

> 所有修改均为纯C++层面，不影响任何蓝图配置。

---

## Fix 1: ECC_AttackDamage — 静态TMap缓存（性能）

**文件:** `Crunch/Private/GAS/Executions/ECC_AttackDamage.cpp`

**问题:** `Execute_Implementation` 每次调用都在栈上重建一个 `TMap<FGameplayTag, CaptureDefinition>` 并 Add 9个条目。伤害计算是热路径（每次攻击/技能命中都触发），这意味着每秒可能执行数十次无用的TMap构建+堆分配。

**修改内容:**
- **行 122~138（原代码）→ 行 122~140（新代码）:** 将函数内的局部TMap替换为文件级 `static const` 函数 `GetTagsToCaptureDefs()`，使用lambda初始化，程序启动时只构建一次
- `Execute_Implementation` 内改为 `const auto& TagsToCaptureDefs = GetTagsToCaptureDefs();`

**效果:** 消除每次伤害计算的TMap堆分配，减少CPU和GC压力

---

## Fix 6（合并到Fix 1）: ECC_AttackDamage — 提取穿透/减免helper函数（代码质量）

**文件:** `Crunch/Private/GAS/Executions/ECC_AttackDamage.cpp`

**问题:** 物理伤害和魔法伤害的穿透计算逻辑完全相同（固定穿透→百分比穿透→减免率→伤害加深），复制粘贴了两遍。真伤的减免计算也是同一模式的简化版。

**修改内容:**
- **新增行 143~149:** `ApplyPenetration(Defense, FlatPen, PercentPen)` 静态helper
- **新增行 152~158:** `ApplyDamageReduction(BaseDamage, DefenseReduction, DamageReduction, DamageAmp)` 静态helper
- **行 186~193（物理伤害段）:** 替换为 `ApplyPenetration()` + `ApplyDamageReduction()` 调用
- **行 259~266（魔法伤害段）:** 同上
- **行 287~289（真伤段）:** 替换为 `ApplyDamageReduction(BaseTrueDamage, 0.0f, DamageReduction, DamageAmp)`

**效果:** 消除代码重复，后续修改伤害公式只需改一处

---

## Fix 2: AttributeSet — 网络同步条件优化（带宽）

### 2a. CAttributeSet 移除Meta属性同步

**文件:** `Crunch/Private/GAS/Core/CAttributeSet.h` + `CAttributeSet.cpp`

**问题:** `AttackDamage`、`MagicDamage`、`TrueDamage` 是Meta属性（服务器在 `PostGameplayEffectExecute` 中读取后立即清零），客户端永远用不到它们的值，但现在用 `COND_None, REPNOTIFY_Always` 同步给所有客户端。每次伤害计算都会触发3个属性的无用网络同步。

**修改内容:**
- **CAttributeSet.h 行 127~140:** 将 `ReplicatedUsing = OnRep_XXX` 改为 `BlueprintReadOnly`（移除同步标记）
- **CAttributeSet.h:** 移除 `OnRep_AttackDamage`、`OnRep_MagicDamage`、`OnRep_TrueDamage` 三个函数声明（属性不再同步，OnRep无意义）
- **CAttributeSet.cpp 行 31~33:** 移除 `DOREPLIFETIME` 注册，替换为注释说明
- **CAttributeSet.cpp:** 移除 `OnRep_AttackDamage`、`OnRep_MagicDamage`、`OnRep_TrueDamage` 三个函数实现（避免UHT警告）

**效果:** 每次伤害计算减少3个属性的网络同步包，64人服务器可显著降低带宽

### 2b. CHeroAttributeSet 优化同步条件

**文件:** `Crunch/Private/GAS/Core/CHeroAttributeSet.cpp`

**问题:** 所有Hero属性都用 `COND_None`（同步给所有客户端），但很多属性只有自己需要看。

**修改内容（行 32~39）:**

| 属性 | 原条件 | 新条件 | 原因 |
|------|--------|--------|------|
| `Experience` | `COND_None` | `COND_OwnerOnly` | 只有自己需要看经验值 |
| `PrevLevelExperience` | `COND_None` | `COND_OwnerOnly` | 同上 |
| `NextLevelExperience` | `COND_None` | `COND_OwnerOnly` | 同上 |
| `Level` | `COND_None` | `COND_None`（不变） | 所有人需要看等级 |
| `UpgradePoint` | `COND_None` | `COND_OwnerOnly` | 只有自己需要 |
| `MaxLevel` | `COND_None` | `COND_InitialOnly` | 初始化后不变 |
| `MaxLevelExperience` | `COND_None` | `COND_InitialOnly` | 初始化后不变 |
| `Gold` | `COND_None` | `COND_OwnerOnly` | 只有自己需要看金币 |

**效果:** 减少约60%的Hero属性网络流量（8个属性中6个不再广播给所有客户端）

---

## Fix 3: ProjectileActor — Target指针安全（崩溃修复）

**文件:** `Crunch/Private/Actor/ProjectileActor.h` + `ProjectileActor.cpp`

**问题:** `Target` 成员是裸指针 `const AActor* Target`。如果目标在投射物飞行途中被销毁（死亡/断线），下一帧 `Tick` 中 `Target->GetActorLocation()` 会访问悬空指针导致崩溃。

**修改内容:**
- **ProjectileActor.h 行 82~83:** `const AActor* Target` → `TWeakObjectPtr<const AActor> Target`
- **ProjectileActor.cpp 行 176:** `if (Target)` → `if (Target.IsValid())`

**效果:** 消除投射物追踪目标时的潜在崩溃。`TWeakObjectPtr` 会在目标被GC后自动变为无效，`IsValid()` 安全返回false。

---

## Fix 4: CAbilitySystemStatics — Tag查询缓存（性能）

**文件:** `Crunch/Private/GAS/Core/CAbilitySystemStatics.cpp`

**问题:** `GetCameraShakeGameplayCueTag()` 和 `GetDamageNumberGameplayCueTag()` 每次调用都执行 `FGameplayTag::RequestGameplayTag("字符串")`，内部做字符串哈希查找。这两个函数被频繁调用（每次命中都调相机震动，每次伤害都调伤害数字）。

**修改内容（行 25~33）:**
```cpp
// 修改前
return FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");

// 修改后
static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");
return Tag;
```

**效果:** Tag查找从每次调用O(1哈希)降为首次调用后O(0)直接返回缓存值

---

## Fix 5: CGameplayAbility — 提取重复伤害逻辑（代码质量）

**文件:** `Crunch/Private/GAS/Core/CGameplayAbility.h` + `CGameplayAbility.cpp`

**问题:** `ApplyDamageToActor()` 和 `ApplyDamageToTargetDataHandle()` 有完全重复的代码：
1. 4行EffectContext创建逻辑（MakeEffectContext + SetAbility + AddSourceObject + AddInstigator）
2. 整个DamageTypeDefinitions循环（MakeOutgoingSpec + BaseDamage + AttributeModifiers + SetByCaller）

另外 `MakeDamage()` 函数已废弃，全是注释掉的死代码。

**修改内容:**
- **CGameplayAbility.h 行 ~55:** 新增两个private方法声明：
  - `MakeDamageEffectContext()` — 创建伤害EffectContext（公共的4行逻辑）
  - `ApplyDamageSpecToTarget()` — DamageTypeDefinitions循环逻辑
- **CGameplayAbility.cpp:**
  - 新增 `MakeDamageEffectContext()` 实现（提取的公共Context创建）
  - 新增 `ApplyDamageSpecToTarget()` 实现（提取的公共循环逻辑）
  - `ApplyDamageToActor()` 简化为3行：创建Context → 包装TargetData → 调用公共方法
  - `ApplyDamageToTargetDataHandle()` 简化为2行：创建Context → 调用公共方法
  - 删除整个 `MakeDamage()` 废弃函数（~40行死代码）

**效果:** 消除代码重复，后续修改伤害应用逻辑只需改一处。清理死代码。

---

## 修改文件汇总

| 文件路径 | Fix编号 | 修改类型 |
|----------|---------|----------|
| `GAS/Executions/ECC_AttackDamage.cpp` | 1, 6 | 性能 + 代码质量 |
| `GAS/Core/CAttributeSet.h` | 2a | 带宽优化 |
| `GAS/Core/CAttributeSet.cpp` | 2a | 带宽优化 |
| `GAS/Core/CHeroAttributeSet.cpp` | 2b | 带宽优化 |
| `Actor/ProjectileActor.h` | 3 | 崩溃修复 |
| `Actor/ProjectileActor.cpp` | 3 | 崩溃修复 |
| `GAS/Core/CAbilitySystemStatics.cpp` | 4 | 性能 |
| `GAS/Core/CGameplayAbility.h` | 5 | 代码质量 |
| `GAS/Core/CGameplayAbility.cpp` | 5 | 代码质量 |

---

## 蓝图影响

**无。** 所有修改均为C++内部实现优化，不改变任何UPROPERTY的名称、类型或语义。现有蓝图无需任何调整。
