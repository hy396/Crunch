# GAS 技能系统

## 概述

Crunch 基于 UE5 的 Gameplay Ability System (GAS) 构建了完整的技能和属性系统。所有战斗相关的逻辑（技能释放、伤害计算、属性修改、Buff/Debuff）都通过 GAS 实现。

## 目录结构

```
GAS/
├── Abilities/       所有技能实现 (GA_*, GAP_*)
├── Core/            核心组件
│   ├── CAbilitySystemComponent.h/.cpp   扩展 ASC
│   ├── CGameplayAbility.h/.cpp          技能基类
│   ├── CGameplayAbilityTypes.h/.cpp     类型定义
│   ├── CAttributeSet.h/.cpp             基础属性集
│   ├── CHeroAttributeSet.h/.cpp         英雄属性集
│   ├── CAbilitySystemStatics.h/.cpp     工具函数
│   ├── TGameplayTags.h/.cpp             标签管理
│   └── GASDebugHelper.h/.cpp            调试工具
├── Data/            PDA_AbilitySystemGenerics (数据资产)
├── Executions/      ECC_AttackDamage (伤害执行计算)
├── MMC/             MMC_AbilityCooldown, MMC_LevelBased
└── TA/              目标检测 Actor
```

## 核心组件

### CAbilitySystemComponent

扩展的 ASC，负责技能的初始化和管理：

- `InitAbilitiesFromCharacterDef()` — 从角色数据资产初始化技能表
- `UpgradeAbility(ECAbilityInputID)` — 升级指定槽位技能
- `ApplyFullStatEffect()` — 应用完整属性初始化效果
- 技能输入绑定通过 `ECAbilityInputID` 枚举映射

### CGameplayAbility

所有自定义技能的基类，提供：

**伤害管线**（重构后的公共逻辑）：
```
MakeDamageEffectContext()           创建伤害上下文
  → SetAbility + AddSourceObject + AddInstigator
ApplyDamageSpecToTarget()           循环 DamageTypeDefinitions
  → MakeOutgoingSpec → SetByCaller(BaseDamage + Modifiers) → ApplyToTargetData
```

两个对外接口：
- `ApplyDamageToActor(AActor*, FGenericDamageEffectDef, Level)` — 对单个 Actor 应用伤害
- `ApplyDamageToTargetDataHandle(FGameplayAbilityTargetDataHandle, FGenericDamageEffectDef, Level)` — 对目标数据应用伤害

**推力系统**：
- `PushCharacterFromLocation(ACCharacter*, FVector Origin, float Force)` — 从指定位置推开角色

**伤害定义结构** (`FGenericDamageEffectDef`)：
```cpp
TSubclassOf<UGameplayEffect> DamageEffect;          // 伤害 GE 类
TMap<FGameplayTag, FDamageTypeDefinition> DamageTypeDefinitions;  // 伤害类型映射
// FDamageTypeDefinition 包含:
//   FScalableFloat BaseDamage;                      // 基础伤害（按等级缩放）
//   TMap<FGameplayTag, FScalableFloat> AttributeModifiers;  // 属性加成系数
```

### 输入映射

```cpp
enum class ECAbilityInputID : uint8
{
    None,
    Confirm,      // 确认
    Cancel,       // 取消
    Ability_LMB,  // 左键（普攻）
    Ability_RMB,  // 右键
    Ability_Q,    // Q 技能
    Ability_E,    // E 技能
    Ability_F,    // F 技能
    Ability_R,    // R 技能（大招）
    // ...
};
```

## 属性系统

### CAttributeSet（基础属性集）

所有角色共用，战斗相关：

| 属性 | 同步 | 说明 |
| ---- | ---- | ---- |
| Health / MaxHealth | Replicated | 生命值 |
| Mana / MaxMana | Replicated | 法力值 |
| AttackPower | Replicated | 物理攻击力 |
| MagicPower | Replicated | 魔法攻击力 |
| Armor | Replicated | 护甲 |
| MagicResistance | Replicated | 魔抗 |
| MoveSpeed | Replicated | 移动速度 |
| ArmorPenetration | Replicated | 固定护甲穿透 |
| ArmorPenetrationPercent | Replicated | 百分比护甲穿透 |
| MagicPenetration | Replicated | 固定魔抗穿透 |
| MagicPenetrationPercent | Replicated | 百分比魔抗穿透 |
| DamageAmplification | Replicated | 伤害加深 |
| DamageReduction | Replicated | 伤害减免 |
| AttackDamage | 不同步 | Meta：物理伤害值（用完清零） |
| MagicDamage | 不同步 | Meta：魔法伤害值（用完清零） |
| TrueDamage | 不同步 | Meta：真实伤害值（用完清零） |

Meta 属性（AttackDamage/MagicDamage/TrueDamage）仅在服务器的 `PostGameplayEffectExecute` 中使用，读取后立即清零，不需要网络同步。

### CHeroAttributeSet（英雄属性集）

仅玩家角色拥有：

| 属性 | 同步条件 | 说明 |
| ---- | ---- | ---- |
| Experience | OwnerOnly | 当前经验 |
| PrevLevelExperience | OwnerOnly | 当前等级起始经验 |
| NextLevelExperience | OwnerOnly | 下一等级所需经验 |
| Level | None（全员） | 等级 |
| UpgradePoint | OwnerOnly | 可用技能点 |
| MaxLevel | InitialOnly | 最大等级（不变） |
| MaxLevelExperience | InitialOnly | 满级经验（不变） |
| Gold | OwnerOnly | 金币 |

## 伤害计算 (ECC_AttackDamage)

执行计算类，处理完整的伤害公式：

```
物理伤害:
  1. 获取 BaseDamage (SetByCaller)
  2. 固定穿透: EffectiveArmor = max(0, Armor - FlatPen)
  3. 百分比穿透: EffectiveArmor *= (1 - PercentPen)
  4. 护甲减免率: Reduction = EffectiveArmor / (EffectiveArmor + 100)
  5. 最终伤害: Damage * (1 - Reduction) * (1 - DamageReduction) * (1 + DamageAmp)

魔法伤害: 同上，使用 MagicResistance 和魔法穿透

真实伤害: 跳过穿透和护甲，仅应用 DamageReduction 和 DamageAmp
```

穿透和减免计算提取为静态 helper 函数（`ApplyPenetration`, `ApplyDamageReduction`），Tag→CaptureDef 映射使用静态 TMap 缓存。

## GameplayTags

所有标签在 `TGameplayTags` 命名空间中集中声明和定义：

```
Attribute.*                 属性标签 (MaxHealth, Health, Mana, ...)
Ability.Combo.*             连击
Ability.Shoot.*             射击
Ability.ChainAttack.*       连锁攻击
Ability.BlackHole.*         黑洞
Ability.WarCry.*            战吼
Ability.Guillotine.*        断头台
Ability.AxeThrow.*          掷斧
Ability.Earthquake.*        地裂（3段 Slam1/2/3）
Ability.ArcaneOrb.*         奥术弹
Ability.PhaseShift.*        相位转移
Ability.MeteorStrike.*      陨石打击
Ability.ArcaneStorm.*       奥术风暴
Ability.Generic.Damage      通用伤害事件
Status.Dead                 死亡
Status.Stunned              眩晕
GameplayCue.CameraShake     镜头震动
GameplayCue.Damage.Number   伤害数字
```

每个技能通常有：主标签、动画事件标签、冷却标签。

## 目标检测 Actor (TA)

| 类 | 用途 | 关键参数 |
| ---- | ---- | ---- |
| `ATargetActor_Around` | 以角色为中心的圆形范围检测 | Radius, bAttachToCharacter |
| `ATargetActor_Line` | 线性/锥形范围检测 | Length, Radius（宽度模拟锥形） |
| `ATargetActor_BlackHole` | 持续吸引+范围检测 | PullSpeed, Radius, Duration, TickInterval |
| `ATargetActor_GroundPick` | 地面选点（鼠标指向） | Range, Radius |

使用方式：在技能中通过 `UAbilityTask_WaitTargetData` 异步等待目标数据。

## 幅度计算 (MMC)

- `MMC_AbilityCooldown` — 根据技能等级计算冷却时间
- `MMC_LevelBased` — 通用等级缩放计算

## 技能列表

### 通用技能

| 技能 | 类型 | 说明 |
| ---- | ---- | ---- |
| GA_Combo | 普攻 | 多段连击 |
| GA_Shoot | 远程 | 远程射击 |
| GA_ChainAttack | 连锁 | 多目标连续攻击 |
| GA_Dash | 位移 | 冲刺 |
| GA_Blink | 位移 | 闪现 |
| GA_Freeze | 控制 | 冰冻 |
| GA_Tornado | 控制 | 龙卷风 |
| GA_BlackHole | 控制 | 黑洞吸引+持续伤害 |
| GA_GroundBlast | 伤害 | 地面冲击 |
| GA_Laser | 伤害 | 激光 |
| GA_SwordAura | 伤害 | 剑气 |
| UpperCut | 伤害 | 上挑 |

### 法师技能

| 技能 | 按键 | 说明 |
| ---- | ---- | ---- |
| GA_ArcaneOrb | Q | 穿透奥术球，距离越远伤害越高，命中回蓝 |
| GA_PhaseShift | E | 无敌后撤+原地放陷阱（定身） |
| GA_MeteorStrike | F | 地面选点陨石+燃烧地面 DOT |
| GA_ArcaneStorm | R | 持续 AOE 减速+最终爆发沉默 |

配套 Actor: `AArcaneOrbProjectile`, `AArcaneTrapActor`, `ABurningGroundActor`

### 战士技能

| 技能 | 按键 | 说明 |
| ---- | ---- | ---- |
| GA_WarCry | Q | AOE 伤害+减甲减速 |
| GA_Guillotine | E | 锥形劈砍+低血斩杀+流血 |
| GA_AxeThrow | F | 回旋飞斧，去程回程双段伤害 |
| GA_Earthquake | R | 三段砸地，前两段眩晕，第三段击飞 |

配套 Actor: `ABoomerangAxeProjectile`

### 被动技能

| 技能 | 说明 |
| ---- | ---- |
| GAP_Dead | 死亡处理：击杀/助攻金币经验分配，连杀连败赏金机制 |
| GAP_Launched | 击飞状态处理 |

## 添加新技能

1. 创建 C++ 类继承 `UCGameplayAbility`，实现 `ActivateAbility` 和 `EndAbility`
2. 在 `TGameplayTags.h/.cpp` 中添加对应标签（主标签、事件标签、冷却标签）
3. 创建所需的 GE 蓝图（伤害、冷却、Buff/Debuff）
4. 创建技能蓝图，配置 `UPROPERTY` 暴露的参数（蒙太奇、GE 引用、伤害定义）
5. 将技能蓝图添加到角色的 `PDA_CharacterDefinition` 技能表中

详细的蓝图配置步骤参考 [GAS_Mage_Abilities.md](GAS_Mage_Abilities.md) 和 [GAS_Warrior_Abilities.md](GAS_Warrior_Abilities.md)。

## 调试

`UGASDebugHelper` 提供以下静态方法：

- `PrintASCState(ASC, bVerbose)` — 打印完整 ASC 快照
- `PrintActiveEffects(ASC)` — 列出所有活跃 GE
- `PrintGrantedAbilities(ASC)` — 列出所有技能及冷却
- `PrintAttributes(ASC)` — 打印所有属性（基础值 vs 当前值）
- `PrintNetworkState(ASC)` — 同步模式和活跃标签
- `RegisterPredictionFailureMonitor(ASC)` — 注册技能激活/失败/结束回调

日志类别: `LogCrunchGAS`，便捷宏: `GAS_LOG()`, `GAS_WARN()`, `GAS_LOG_ASC()`
