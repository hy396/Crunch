# Crunch GAS系统详细指南

## 📋 概览

Crunch项目基于Unreal Engine 5的Gameplay Ability System (GAS)构建了一套完整的技能系统。本文档详细介绍了项目中GAS系统的架构、组件和使用方法。

## 🏗️ 系统架构

### 核心组件结构

```
GAS/
├── 📁 Core/                    # 核心系统
│   ├── CAbilitySystemComponent # 能力系统组件
│   ├── CGameplayAbility        # 技能基类
│   ├── CAttributeSet           # 属性集合
│   ├── CHeroAttributeSet       # 英雄属性集合
│   ├── CGameplayAbilityTypes   # 技能类型定义
│   ├── TGameplayTags           # GameplayTags管理
│   └── CAbilitySystemStatics   # 静态工具类
├── 📁 Abilities/               # 具体技能实现
│   ├── GA_Shoot               # 射击技能
│   ├── GA_Dash                # 冲刺技能
│   ├── GA_BlackHole           # 黑洞技能
│   ├── GA_Laser               # 激光技能
│   ├── GA_Combo               # 连击技能
│   ├── GA_Blink               # 闪烁技能
│   ├── GA_Freeze              # 冰冻技能
│   ├── GA_GroundBlast         # 地面爆炸技能
│   ├── GA_Tornado             # 龙卷风技能
│   ├── UpperCut               # 升龙拳技能
│   ├── GAP_Dead               # 死亡被动技能（奖励分配）
│   └── GAP_Launched           # 击飞被动技能
├── 📁 Data/                   # 数据资产
│   └── PDA_AbilitySystemGenerics # 通用能力系统配置
├── 📁 Executions/             # 效果执行计算
│   └── ECC_AttackDamage       # 攻击伤害计算
├── 📁 MMC/                    # 数值修改计算
│   ├── MMC_AbilityCooldown    # 技能冷却计算
│   └── MMC_LevelBased         # 等级基础计算
└── 📁 TA/                     # 目标选择器
    ├── TargetActor_Line       # 线性目标检测
    ├── TargetActor_Around     # 环形目标检测
    ├── TargetActor_BlackHole  # 黑洞目标检测
    └── TargetActor_GroundPick # 地面点选目标检测
```

## 🔧 核心组件详解

### 1. CAbilitySystemComponent

**职责**: 扩展UE5的AbilitySystemComponent，提供项目特定的功能

**核心功能**:
```cpp
class UCAbilitySystemComponent : public UAbilitySystemComponent
{
public:
    // 初始化基础属性
    void InitializeBaseAttributes();
    
    // 服务器初始化
    void ServerSideInit();
    
    // 回满血、满蓝效果
    void ApplyFullStatEffect();
    
    // 获取技能映射
    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
    
    // 是否达到最大等级
    bool IsAtMaxLevel() const;
    
    // 技能升级（网络RPC）
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_UpgradeAbilityWithID(ECAbilityInputID InputID);
    
    // 客户端技能等级同步
    UFUNCTION(Client, Reliable)
    void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int NewLevel);
    
private:
    // 基础技能映射
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;
    
    // 升级技能映射
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;
    
    // 通用配置数据资产
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
    TObjectPtr<UPDA_AbilitySystemGenerics> AbilitySystemGenerics;
};
```

### 2. CGameplayAbility

**职责**: 技能基类，提供所有技能的通用功能

**核心功能**:
```cpp
class UCGameplayAbility : public UGameplayAbility
{
public:
    // 技能冷却配置
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
    FScalableFloat CooldownDuration;
    
protected:
    // 目标检测与选择
    AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;
    
    // 效果应用
    void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, 
                                           TSubclassOf<UGameplayEffect> GameplayEffect, 
                                           int Level = 1);
    
    // 伤害应用
    void ApplyDamageToActor(AActor* TargetActor, 
                           const FGenericDamageEffectDef& Damage, 
                           int Level = 1);
    
    // 推动效果
    void PushTarget(AActor* Target, const FVector& PushVel);
    void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel);
    
    // 动画播放
    void PlayMontageLocally(UAnimMontage* MontageToPlay);
    
    // 团队系统
    FGenericTeamId GetOwnerTeamId() const;
    bool IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const;
    
    // 调试开关
    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bShouldDrawDebug = false;
};
```

### 3. 属性系统

#### CAttributeSet
项目的主要属性集合，包含：

```cpp
// 基础属性
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
FGameplayAttributeData Health;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
FGameplayAttributeData MaxHealth;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
FGameplayAttributeData Mana;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
FGameplayAttributeData MaxMana;

// 伤害属性
UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
FGameplayAttributeData AttackDamage;

UPROPERTY(ReplicatedUsing = OnRep_MagicDamage)
FGameplayAttributeData MagicDamage;

UPROPERTY(ReplicatedUsing = OnRep_TrueDamage)
FGameplayAttributeData TrueDamage;

// 战斗属性
UPROPERTY(ReplicatedUsing = OnRep_AttackPower)
FGameplayAttributeData AttackPower;

UPROPERTY(ReplicatedUsing = OnRep_MagicPower)
FGameplayAttributeData MagicPower;

UPROPERTY(ReplicatedUsing = OnRep_Armor)
FGameplayAttributeData Armor;

UPROPERTY(ReplicatedUsing = OnRep_MagicResistance)
FGameplayAttributeData MagicResistance;

// 移动属性
UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
FGameplayAttributeData MoveSpeed;

UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
FGameplayAttributeData MoveAcceleration;
```

#### CHeroAttributeSet
英雄专用的额外属性集合（扩展属性）

**新增属性**:
```cpp
// 连续击杀/死亡属性
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_KillStreak)
FGameplayAttributeData KillStreak;

UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DeathStreak)
FGameplayAttributeData DeathStreak;
```

### 4. TGameplayTags管理系统

**职责**: 统一管理项目中所有的GameplayTags

**分类结构**:
```cpp
namespace TGameplayTags {
    // 基础攻击技能
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_BasicAttack)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_BasicAttack_Pressed)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_BasicAttack_Released)
    
    // 连击系统
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo01)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo02)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo03)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo04)
    
    // 特殊技能
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Shoot)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Laser_Shoot)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_BlackHole_Cooldown)
    
    // 状态标签
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stats_Dead)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stats_Stun)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stats_Aim)
    
    // 伤害类型
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageType_AttackDamage)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageType_MagicDamage)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageType_TrueDamage)
    
    // 属性标签
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Mana)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_AttackPower)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_MagicPower)
}
```

## 🎯 技能系统详解

### 技能分类

#### 1. 主动攻击技能
- **GA_Shoot**: 射击技能，支持连续射击和目标追踪
- **GA_Laser**: 激光技能，线性伤害
- **GA_BlackHole**: 黑洞技能，范围控制
- **GA_GroundBlast**: 地面爆炸，范围伤害
- **GA_Tornado**: 龙卷风，移动范围伤害
- **UpperCut**: 升龙拳，单体高伤害

#### 2. 移动技能
- **GA_Dash**: 冲刺技能，快速位移
- **GA_Blink**: 闪烁技能，瞬间传送

#### 3. 控制技能
- **GA_Freeze**: 冰冻技能，目标控制
- **GA_Combo**: 连击系统，组合攻击

#### 4. 被动技能
- **GAP_Dead**: 死亡触发被动（奖励分配系统）
- **GAP_Launched**: 击飞状态被动

### 死亡奖励系统 (GAP_Dead)

#### 职责
处理角色死亡时的奖励分配（经验、金币、击杀数等）

#### 核心功能
- 动态赏金计算（基于KillStreak和DeathStreak）
- 英雄击杀奖励分配（击杀者+助攻者）
- 小兵击杀奖励分配（范围检测+比例分配）
- 连续击杀/死亡属性更新
- 使用Batch操作优化GE应用性能

#### 赏金计算公式
```
总赏金 = Clamp(BaseBounty + (KillStreak * BountyPerStreak) - (DeathStreak * PenaltyPerDeath), MinBounty, MaxBounty)
```

#### 奖励分配逻辑
- **英雄击杀**:
  - 击杀者获得总赏金
  - 每个助攻者获得总赏金/2
  - 所有参与者获得连杀/连败更新
- **小兵击杀**:
  - 击杀者获得KillerRewardPortion比例奖励
  - 队友平分剩余部分
  - 范围限制：RewardRange内的队友

#### UDeadEventPayload事件负载类
**职责**: 用于传递死亡事件数据的负载类

```cpp
class UDeadEventPayload : public UObject
{
public:
    /** 击杀者（最后造成伤害的英雄） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<AActor> Killer;

    /** 助攻者列表（所有符合条件的伤害来源，除了击杀者） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TObjectPtr<AActor>> AssistHeroes;
};
```

### 技能实现示例：GA_Shoot

```cpp
class UGA_Shoot : public UCGameplayAbility
{
public:
    // 激活能力
    virtual void ActivateAbility(...) override;
    
    // 输入释放
    virtual void InputReleased(...) override;
    
    // 结束能力
    virtual void EndAbility(...) override;
    
private:
    // 伤害配置
    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    FGenericDamageEffectDef ProjectileHitEffect;
    
    // 射击参数
    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    float ShootProjectileSpeed = 2000.f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    float ShootProjectileRange = 3000.f;
    
    // 子弹类
    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    TSubclassOf<AProjectileActor> ProjectileClass;
    
    // 动画
    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    TObjectPtr<UAnimMontage> ShootMontage;
    
    // 目标系统
    UPROPERTY()
    TObjectPtr<AActor> AimTarget;
    
    FTimerHandle AimTargetCheckTimerHandle;
    
    // 目标检测
    void FindAimTarget();
    AActor* GetAimTargetIfValid() const;
    void TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount);
};
```

## 🎯 目标选择系统

### TargetActor类型

#### 1. TargetActor_Line
**用途**: 线性技能目标检测（如激光、射击）

**功能**:
```cpp
class ATargetActor_Line : public AGameplayAbilityTargetActor
{
public:
    // 配置检测参数
    void ConfigureTargetSetting(
        float NewTargetRange,
        float NewDetectionCylinderRadius,
        float NewTargetingInterval,
        FGenericTeamId OwnerTeamId,
        bool bShouldDrawDebug
    );
    
private:
    // 检测参数
    UPROPERTY(Replicated)
    float TargetRange;
    
    UPROPERTY(Replicated)
    float DetectionCylinderRadius;
    
    // 特效组件
    UPROPERTY(VisibleDefaultsOnly, Category = "Component")
    TObjectPtr<UParticleSystemComponent> LaserFX;
    
    // 检测逻辑
    void DoTargetCheckAndReport();
    void UpdateTargetTrace();
    bool ShouldReportActorAsTarget(const AActor* ActorToCheck) const;
};
```

#### 2. TargetActor_Around
**用途**: 环形范围技能目标检测

#### 3. TargetActor_BlackHole
**用途**: 黑洞技能专用目标检测

#### 4. TargetActor_GroundPick
**用途**: 地面点选目标检测

## 📊 数值计算系统

### MMC (Magnitude Calculation)

#### 1. MMC_AbilityCooldown
**职责**: 技能冷却时间计算

```cpp
class UMMC_AbilityCooldown : public UGameplayModMagnitudeCalculation
{
public:
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
    
private:
    // 冷却减少属性捕获
    FGameplayEffectAttributeCaptureDefinition CooldownReductionCaptureDef;
};
```

#### 2. MMC_LevelBased
**职责**: 基于等级的数值计算

### Execution Calculation

#### ECC_AttackDamage
**职责**: 攻击伤害的复杂计算

```cpp
class UECC_AttackDamage : public UGameplayEffectExecutionCalculation
{
public:
    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput
    ) const override;
};
```

## 📦 数据资产系统

### UPDA_AbilitySystemGenerics

**职责**: 存储所有角色共用的能力系统配置

```cpp
class UPDA_AbilitySystemGenerics : public UPrimaryDataAsset
{
public:
    // 获取完整属性效果
    TSubclassOf<UGameplayEffect> GetFullStatEffect() const;
    
    // 获取死亡效果
    TSubclassOf<UGameplayEffect> GetDeathEffect() const;
    
    // 获取初始效果数组
    const TArray<TSubclassOf<UGameplayEffect>>& GetInitialEffects() const;
    
    // 获取被动技能数组
    const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const;
    
    // 获取基础属性数据表
    const UDataTable* GetBaseStatDataTable() const;
    
    // 获取经验曲线
    const FRealCurve* GetExperienceCurve() const;
    
private:
    // 配置数据
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> FullStatEffect;
    
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> DeathEffect;
    
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
    
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
    TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
    
    UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
    TObjectPtr<UDataTable> BaseStatDataTable;
    
    // 经验曲线配置
    UPROPERTY(EditDefaultsOnly, Category = "Level")
    FName ExperienceRowName = "ExperienceNeededToReachLevel";
    
    UPROPERTY(EditDefaultsOnly, Category = "Level")
    TObjectPtr<UCurveTable> ExperienceCurveTable;
};
```

## 🔧 使用指南

### 1. 创建新技能

1. **继承基类**
```cpp
UCLASS()
class CRUNCH_API UGA_MyNewAbility : public UCGameplayAbility
{
    GENERATED_BODY()
public:
    UGA_MyNewAbility();
    
    virtual void ActivateAbility(...) override;
    virtual void EndAbility(...) override;
    
private:
    // 技能特定配置
    UPROPERTY(EditDefaultsOnly, Category = "MyAbility")
    FGenericDamageEffectDef DamageEffect;
};
```

2. **在TGameplayTags.h中添加标签**
```cpp
namespace TGameplayTags {
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_MyNewAbility)
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_MyNewAbility_Cooldown)
}
```

3. **在TGameplayTags.cpp中定义标签**
```cpp
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_MyNewAbility, "Ability.MyNewAbility", "我的新技能")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_MyNewAbility_Cooldown, "Ability.MyNewAbility.Cooldown", "我的新技能冷却")
```

4. **在CAbilitySystemComponent中注册**
```cpp
// 在蓝图或数据资产中配置技能映射
BasicAbilities.Add(ECAbilityInputID::Ability1, UGA_MyNewAbility::StaticClass());
```

### 2. 自定义目标选择器

```cpp
UCLASS()
class ATargetActor_MyCustom : public AGameplayAbilityTargetActor
{
    GENERATED_BODY()
public:
    virtual void StartTargeting(UGameplayAbility* Ability) override;
    virtual void Tick(float DeltaTime) override;
    
private:
    void DoCustomTargeting();
};
```

### 3. 添加新属性

1. **在CAttributeSet.h中声明**
```cpp
UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MyNewAttribute)
FGameplayAttributeData MyNewAttribute;
ATTRIBUTE_ACCESSORS(UCAttributeSet, MyNewAttribute)

UFUNCTION()
void OnRep_MyNewAttribute(const FGameplayAttributeData& OldValue);
```

2. **在对应的cpp文件中实现网络同步**
```cpp
void UCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MyNewAttribute, COND_None, REPNOTIFY_Always);
}

void UCAttributeSet::OnRep_MyNewAttribute(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MyNewAttribute, OldValue);
}
```

## 🚀 最佳实践

### 1. 性能优化
- 使用对象池管理频繁创建的TargetActor
- 合理设置技能检测频率，避免每帧检测
- 使用属性捕获而不是直接访问属性值
- 适当使用GameplayTags进行状态管理

### 2. 网络同步
- 所有重要的技能效果都应通过GameplayEffect应用
- 使用Server RPC进行技能激活验证
- 客户端预测与服务器校验相结合
- 属性变化使用RepNotify确保同步

### 3. 调试技巧
- 使用bShouldDrawDebug开关控制调试显示
- 善用GameplayTags进行状态跟踪
- 利用GAS自带的调试命令（showdebug abilitysystem）
- 在关键位置添加UE_LOG输出

### 4. 扩展性设计
- 通过数据资产配置技能参数
- 使用接口解耦不同系统
- 遵循单一职责原则，每个类专注一个功能
- 预留扩展点，方便后续功能添加

## 🎯 总结

Crunch的GAS系统采用了模块化设计，将技能、属性、目标选择、数值计算等功能分离到不同的模块中。通过统一的标签管理系统和数据资产配置，实现了灵活且易于扩展的技能系统架构。

系统支持：
- ✅ 多样化的技能类型（攻击、移动、控制、被动）
- ✅ 灵活的目标选择机制
- ✅ 完整的属性系统和数值计算
- ✅ 网络同步和客户端预测
- ✅ 可视化调试和特效支持
- ✅ 数据驱动的配置系统

这套GAS系统为Crunch项目提供了强大的技能基础，支持复杂的多人对战玩法需求。

---

> 🎮 **Crunch GAS系统** - 强大、灵活、易扩展的技能系统！
> 
> 📝 本文档基于真实源码编写，确保内容准确性。