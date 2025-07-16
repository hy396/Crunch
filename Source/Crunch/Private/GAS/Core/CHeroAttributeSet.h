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
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, PrevLevelExperience)      // 上一级所需经验
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, NextLevelExperience)      // 下一级所需经验
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Level)                    // 当前等级
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, UpgradePoint)             // 可用升级点
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MaxLevel)                 // 最大等级
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, MaxLevelExperience)       // 满级所需经验
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, Gold)                     // 金币
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, StrengthGrowthRate)       // 力量成长率
    ATTRIBUTE_ACCESSORS(UCHeroAttributeSet, IntelligenceGrowthRate)   // 智力成长率

    // 属性同步（网络复制）配置
    virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
private:
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

    // 上一级所需经验
    UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExperience)
    FGameplayAttributeData PrevLevelExperience;

    // 下一级所需经验
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
};