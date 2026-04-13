# Walkthrough: 添加新英雄技能

[返回首页](Home.md)

本文以法师的「奥术弹」(GA_ArcaneOrb) 为实际范例，完整走一遍添加新技能的全流程。

## 前置知识

- 熟悉 GAS 基本概念（Ability、GameplayEffect、GameplayTag）
- 了解项目的技能基类 `UCGameplayAbility`，详见 [docs/GAS_SYSTEM.md](../docs/GAS_SYSTEM.md)

## Step 1: 创建技能 C++ 类

**目标文件**: `Source/Crunch/Private/GAS/Abilities/GA_ArcaneOrb.h` 和 `.cpp`

```cpp
// GA_ArcaneOrb.h
#pragma once
#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_ArcaneOrb.generated.h"

class AArcaneOrbProjectile;  // 前向声明配套 Actor

UCLASS()
class UGA_ArcaneOrb : public UCGameplayAbility
{
    GENERATED_BODY()
public:
    UGA_ArcaneOrb();

    virtual void ActivateAbility(...) override;
    virtual void EndAbility(...) override;

protected:
    // 蓝图配置 - 动画蒙太奇
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* CastMontage;

    // 蓝图配置 - 投射物类
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<AArcaneOrbProjectile> ProjectileClass;

    // 蓝图配置 - 伤害定义
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    FGenericDamageEffectDef DamageDefinition;

    // 蓝图配置 - 回蓝效果
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> ManaRestoreEffect;
};
```

**参考**: 现有的 `GA_BlackHole.h` 结构类似。

### 关键实现模式

```cpp
void UGA_ArcaneOrb::ActivateAbility(...)
{
    // 1. 播放蒙太奇
    auto* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(...);
    MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);

    // 2. 等待动画事件（AnimNotify 发出的 GameplayTag 事件）
    auto* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(...);
    EventTask->EventReceived.AddDynamic(this, &ThisClass::OnCastEvent);

    // 3. 在 OnCastEvent 中生成投射物
    // 4. 投射物命中时调用 ApplyDamageToActor()
}
```

## Step 2: 创建配套 Actor（如需要）

如果技能需要投射物、陷阱、持续区域等，在 `Source/Crunch/Private/Actor/` 下创建。

**目标文件**: `Source/Crunch/Private/Actor/ArcaneOrbProjectile.h` 和 `.cpp`

奥术弹需要一个穿透投射物，继承现有的 `AProjectileActor`：

```cpp
UCLASS()
class AArcaneOrbProjectile : public AProjectileActor
{
    GENERATED_BODY()
    // 覆写 OnOverlap, 实现穿透逻辑（不销毁，记录已命中目标）
};
```

**参考**: `AProjectileActor`（基类）、`ABoomerangAxeProjectile`（回旋投射物范例）

其他配套 Actor 范例：
- `AArcaneTrapActor` - 延迟激活陷阱（AActor + IGenericTeamAgentInterface）
- `ABurningGroundActor` - 持续区域伤害（AActor + 周期 Timer）

## Step 3: 添加 GameplayTag

**目标文件**:
- `Source/Crunch/Private/GAS/Core/TGameplayTags.h`
- `Source/Crunch/Private/GAS/Core/TGameplayTags.cpp`

头文件中声明：

```cpp
// TGameplayTags.h - 在对应区域添加
CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ArcaneOrb)
CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ArcaneOrb_Cast)      // 施法动画事件
CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ArcaneOrb_Cooldown)  // 冷却标签
```

实现文件中定义：

```cpp
// TGameplayTags.cpp
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ArcaneOrb, "Ability.ArcaneOrb", "奥术弹技能")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ArcaneOrb_Cast, "Ability.ArcaneOrb.Cast", "施法动画事件")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ArcaneOrb_Cooldown, "Ability.ArcaneOrb.Cooldown", "冷却")
```

**命名规则**: `Ability.技能名.事件名`，冷却统一用 `.Cooldown` 后缀。

## Step 4: 创建 GameplayEffect 蓝图

在编辑器中创建以下 GE 蓝图：

### 伤害 GE
- 基于 `GameplayEffect`
- Duration Policy: `Instant`
- Executions: 添加 `ECC_AttackDamage`
- 这个 GE 类会被技能蓝图的 `DamageDefinition.DamageEffect` 引用

### 冷却 GE
- Duration Policy: `HasDuration`
- Duration Magnitude: 使用 `MMC_AbilityCooldown`（按技能等级缩放）
- Granted Tags: 添加 `Ability.ArcaneOrb.Cooldown`

### Buff/Debuff GE（如需要）
- 根据技能需求创建（减速、减甲、回蓝等）

## Step 5: 创建技能蓝图

1. 在编辑器中创建蓝图，父类选择刚创建的 `GA_ArcaneOrb`
2. 在 Details 面板配置 C++ 中暴露的 `UPROPERTY`：
   - `CastMontage` - 指向施法动画蒙太奇
   - `ProjectileClass` - 指向投射物蓝图
   - `DamageDefinition` - 配置伤害 GE 类和伤害数值
   - `ManaRestoreEffect` - 指向回蓝 GE
3. 配置 GAS 基础属性：
   - `AbilityTags` - 添加 `Ability.ArcaneOrb`
   - `CooldownGameplayEffectClass` - 指向冷却 GE
   - `CostGameplayEffectClass` - 指向消耗 GE（如有法力消耗）

## Step 6: 创建 AnimMontage

1. 创建蒙太奇，添加施法动画片段
2. 在关键帧添加 `AnimNotify_GameplayEvent`
3. 设置 Notify 的 Tag 为 `Ability.ArcaneOrb.Cast`（对应 Step 3 定义的事件标签）

技能 C++ 代码中的 `WaitGameplayEvent` 会监听这个标签，在动画播放到该帧时触发施法逻辑。

## Step 7: 注册到角色

**目标**: 角色的 `UPDA_CharacterDefinition` 数据资产

在编辑器中打开角色定义资产，找到 `AbilityTable`（`TMap<ECAbilityInputID, TSubclassOf<UCGameplayAbility>>`），添加：

| InputID | Ability Class |
| ------- | ------------- |
| `Ability_Q` | `BP_GA_ArcaneOrb`（Step 5 创建的蓝图） |

## Step 8: 编译验证

```bash
Engine/Build/BatchFiles/Build.bat CrunchEditor Win64 Development -project="<path>/Crunch.uproject"
```

常见问题见 [Troubleshooting](Troubleshooting.md)。

验证清单：
- [ ] 编译通过，UHT 成功生成 .generated.h
- [ ] 编辑器中能看到新的技能蓝图
- [ ] PIE 测试中按 Q 能激活技能
- [ ] 投射物正确生成和移动
- [ ] 伤害正确计算和扣血
- [ ] 冷却正常触发

## 文件清单总结

| 步骤 | 文件 | 操作 |
| ---- | ---- | ---- |
| 1 | `GAS/Abilities/GA_ArcaneOrb.h/.cpp` | 新建 |
| 2 | `Actor/ArcaneOrbProjectile.h/.cpp` | 新建（如需） |
| 3 | `GAS/Core/TGameplayTags.h/.cpp` | 修改（添加标签） |
| 4 | Content 蓝图 | 新建 GE 蓝图 |
| 5 | Content 蓝图 | 新建技能蓝图 |
| 6 | Content 蒙太奇 | 新建/修改 |
| 7 | Content 数据资产 | 修改角色定义 |
