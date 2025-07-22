// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CHeroAttributeSet.generated.h"

// 属性访问器宏，自动生成属性的Getter/Setter等
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
     GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
     GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
     GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
     GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 英雄属性集（UCHeroAttributeSet）
 * 用于管理英雄的成长属性、经验、等级、升级点、金币等
 * 支持属性同步、属性变化回调等功能
 */
UCLASS()
class UCHeroAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    // 属性访问器声明（自动生成标准接口）
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Intelligence)             // 智力
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Strength)                 // 力量
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Experience)               // 当前经验
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, PrevLevelExperience)      // 当前等级经验阈值
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, NextLevelExperience)      // 下一级经验阈值
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Level)                    // 当前等级
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, UpgradePoint)             // 可用升级点
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MaxLevel)                 // 最大等级
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MaxLevelExperience)       // 满级所需经验
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Gold)                     // 金币
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, StrengthGrowthRate)       // 力量成长率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, IntelligenceGrowthRate)   // 智力成长率

    // 英雄的属性配置
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, AttackPowerGrowthRate)      // 攻击力成长率（每等级增加的攻击力数值）
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MagicPowerGrowthRate)       // 法术强度成长率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, ArmorGrowthRate)            // 护甲成长
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MagicResistanceGrowthRate)  // 法术抗性成长
   
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MaxHealthGrowthRate)    // 生命成长
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MaxManaGrowthRate)      // 法力成长
    
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, HealthRegen)            // 生命回复速率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, HealthRegenGrowthRate)  // 生命回复速率成长率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, ManaRegen)              // 法力回复速率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, ManaRegenGrowthRate)    // 法力回复速率成长率
    
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, ArmorPenetration)       // 护甲穿透
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MagicPenetration)       // 法术穿透

    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, ArmorPenetrationPercent)// 护甲穿透百分比
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MagicPenetrationPercent)// 法术穿透百分比
    
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, LifeSteal)              // 生命偷取
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MagicLifeSteal)         // 法术吸血
    
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, CooldownReduction)      // 冷却缩减
    
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, CriticalStrikeChance)   // 暴击率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, CriticalStrikeDamage)   // 暴击伤害
    
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Toughness)              // 韧性
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, DamageAmplification)    // 伤害加深（增加对目标造成的伤害百分比）
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, DamageReduction)        // 伤害减免（减少受到的伤害百分比）
    

    // 属性同步（网络复制）配置
    virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
// private:
    // 智力
    UPROPERTY(ReplicatedUsing = OnRep_Intelligence)
    FGameplayAttributeData Intelligence;

    // 力量
    UPROPERTY(ReplicatedUsing = OnRep_Strength)
    FGameplayAttributeData Strength;
    
    // 当前经验
    UPROPERTY(ReplicatedUsing = OnRep_Experience)
    FGameplayAttributeData Experience;

    // 力量成长率
    UPROPERTY()
    FGameplayAttributeData StrengthGrowthRate;
    
    // 智力成长率
    UPROPERTY()
    FGameplayAttributeData IntelligenceGrowthRate;

    /** 攻击力成长率（每等级增加的攻击力数值） */
    UPROPERTY()
    FGameplayAttributeData AttackPowerGrowthRate;
    /** 法术强度成长率（每等级增加的法术强度数值） */
    UPROPERTY()
    FGameplayAttributeData MagicPowerGrowthRate;
    // 护甲成长
    UPROPERTY()
    FGameplayAttributeData ArmorGrowthRate;
    // 法术抗性成长
    UPROPERTY()
    FGameplayAttributeData MagicResistanceGrowthRate;
    
    /** 生命成长率（每等级增加的生命数值） */
    UPROPERTY()
    FGameplayAttributeData MaxHealthGrowthRate;
    /** 法力成长率（每等级增加的法力数值） */
    UPROPERTY()
    FGameplayAttributeData MaxManaGrowthRate;

    /** 生命回复速率（每秒回复的生命值） */
    UPROPERTY(ReplicatedUsing = OnRep_HealthRegen)
    FGameplayAttributeData HealthRegen;
    /** 生命回复速率成长率（每等级增加的生命回复速率） */
    UPROPERTY()
    FGameplayAttributeData HealthRegenGrowthRate;
    /** 法力回复速率（每秒回复的法力值） */
    UPROPERTY(ReplicatedUsing = OnRep_ManaRegen)
    FGameplayAttributeData ManaRegen;
    /** 法力回复速率成长率（每等级增加的法力回复速率） */
    UPROPERTY()
    FGameplayAttributeData ManaRegenGrowthRate;

    /** 护甲穿透（减少目标护甲的效果） */
    UPROPERTY(ReplicatedUsing = OnRep_ArmorPenetration)
    FGameplayAttributeData ArmorPenetration;

    /** 法术穿透（减少目标法术抗性的效果） */
    UPROPERTY(ReplicatedUsing = OnRep_MagicPenetration)
    FGameplayAttributeData MagicPenetration;

    /** 护甲穿透百分比（减少目标护甲的百分比效果） */
    UPROPERTY(ReplicatedUsing = OnRep_ArmorPenetrationPercent)
    FGameplayAttributeData ArmorPenetrationPercent;

    /** 法术穿透百分比（减少目标法术抗性的百分比效果） */
    UPROPERTY(ReplicatedUsing = OnRep_MagicPenetrationPercent)
    FGameplayAttributeData MagicPenetrationPercent;

    /** 生命偷取（造成物理伤害时回复生命值的百分比） */
    UPROPERTY(ReplicatedUsing = OnRep_LifeSteal)
    FGameplayAttributeData LifeSteal;

    /** 法术吸血（造成法术伤害时回复生命值的百分比） */
    UPROPERTY(ReplicatedUsing = OnRep_MagicLifesteal)
    FGameplayAttributeData MagicLifeSteal;

    /** 冷却缩减（减少技能冷却时间的百分比） */
    UPROPERTY(ReplicatedUsing = OnRep_CooldownReduction)
    FGameplayAttributeData CooldownReduction;

    /** 暴击率（普通攻击暴击的几率） */
    UPROPERTY(ReplicatedUsing = OnRep_CriticalStrikeChance)
    FGameplayAttributeData CriticalStrikeChance;

    // 暴击伤害
    UPROPERTY(ReplicatedUsing = OnRep_CriticalStrikeDamage)
    FGameplayAttributeData CriticalStrikeDamage;
    
    /** 韧性（减少被控制效果影响的时间） */
    UPROPERTY(ReplicatedUsing = OnRep_Toughness)
    FGameplayAttributeData Toughness;

    /** 伤害加深（增加对目标造成的伤害百分比） */
    UPROPERTY(ReplicatedUsing = OnRep_DamageAmplification)
    FGameplayAttributeData DamageAmplification;

    /** 伤害减免（减少受到的伤害百分比） */
    UPROPERTY(ReplicatedUsing = OnRep_DamageReduction)
    FGameplayAttributeData DamageReduction;

    // 当前等级经验阈值
    UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExperience)
    FGameplayAttributeData PrevLevelExperience;

    // 下一级经验阈值
    UPROPERTY(ReplicatedUsing = OnRep_NextLevelExperience)
    FGameplayAttributeData NextLevelExperience;

    // 当前等级
    UPROPERTY(ReplicatedUsing = OnRep_Level)
    FGameplayAttributeData Level;
    
    // 可用升级点
    UPROPERTY(ReplicatedUsing = OnRep_UpgradePoint)
    FGameplayAttributeData UpgradePoint;
    
    // 最大等级
    UPROPERTY(ReplicatedUsing = OnRep_MaxLevel)
    FGameplayAttributeData MaxLevel;

    // 满级所需经验
    UPROPERTY(ReplicatedUsing = OnRep_MaxLevelExperience)
    FGameplayAttributeData MaxLevelExperience;

    // 金币
    UPROPERTY(ReplicatedUsing = OnRep_Gold)
    FGameplayAttributeData Gold;

    // 属性同步回调（用于客户端属性变化通知）
    UFUNCTION()
    void OnRep_Intelligence(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Strength(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Experience(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Level(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_UpgradePoint(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MaxLevel(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_Gold(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_HealthRegen(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_ManaRegen(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MagicPenetration(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MagicPenetrationPercent(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_LifeSteal(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MagicLifeSteal(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_CooldownReduction(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_CriticalStrikeDamage(const FGameplayAttributeData& OldValue);
    
    UFUNCTION()
    void OnRep_Toughness(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_DamageAmplification(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_DamageReduction(const FGameplayAttributeData& OldValue);
};