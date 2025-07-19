// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CGameplayAbilityTypes.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ECAbilityInputID : uint8
{
	None							UMETA(DisplayName="None"),
	BasicAttack						UMETA(DisplayName="基础攻击"),
	Aim								UMETA(DisplayName="瞄准"),
	AbilityOne						UMETA(DisplayName="一技能"),
	AbilityTwo						UMETA(DisplayName="二技能"),
	AbilityThree					UMETA(DisplayName="三技能"),
	AbilityFour						UMETA(DisplayName="四技能"),
	AbilityFive						UMETA(DisplayName="五技能"),
	AbilitySix						UMETA(DisplayName="六技能"),
	Confirm							UMETA(DisplayName="确认"),
	Cancel							UMETA(DisplayName="取消")
};

// 伤害效果定义
USTRUCT(BlueprintType)
struct FGenericDamageEffectDef
{
	GENERATED_BODY()

public:
	FGenericDamageEffectDef();

	// 伤害类型
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	// 基础伤害大小
	UPROPERTY(EditAnywhere)
	float BaseDamage;

	// 属性的百分比伤害加成
	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, float> DamageTypes;

	// 力的大小
	UPROPERTY(EditAnywhere)
	FVector PushVelocity;
};

// 英雄基础属性结构体（用于数据表）
USTRUCT(BlueprintType)
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()
public:
	FHeroBaseStats();

	// 英雄类
	UPROPERTY(EditAnywhere, meta=(DisplayName="英雄类"))
	TSubclassOf<AActor> Class;

	// 力量
	UPROPERTY(EditAnywhere, meta=(DisplayName="力量"))
	float Strength;

	// 智力
	UPROPERTY(EditAnywhere, meta=(DisplayName="智力"))
	float Intelligence;
    
	// 力量成长率
	UPROPERTY(EditAnywhere, meta=(DisplayName="力量成长率"))
	float StrengthGrowthRate;

	// 智力成长率
	UPROPERTY(EditAnywhere, meta=(DisplayName="智力成长率"))
	float IntelligenceGrowthRate;

	// 基础最大生命
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础最大生命"))
	float BaseMaxHealth;

	// 基础最大法力
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础最大法力"))
	float BaseMaxMana;

	// 基础攻击力
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础攻击力"))
	float BaseAttackDamage;

	// 基础护甲
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础护甲"))
	float BaseArmor;

	// 基础移动速度
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础移动速度"))
	float BaseMoveSpeed;
};

// 英雄基础属性结构体（用于数据表）
USTRUCT(BlueprintType)
struct FTHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()
public:
	FTHeroBaseStats();

	// 英雄类
	UPROPERTY(EditAnywhere, meta=(DisplayName="英雄类"))
	TSubclassOf<AActor> Class;

	// 攻击力
	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击力"))
	float AttackPower;

	// 法术强度
	UPROPERTY(EditAnywhere, meta=(DisplayName="法术强度"))
	float MagicPower;
    
	// 攻击成长率
	UPROPERTY(EditAnywhere, meta=(DisplayName="攻击成长率"))
	float AttackPowerGrowthRate;

	// 法术成长率
	UPROPERTY(EditAnywhere, meta=(DisplayName="法术成长率"))
	float MagicPowerGrowthRate;

	// 基础最大生命
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础最大生命"))
	float BaseMaxHealth;

	// 生命回复
	UPROPERTY(EditAnywhere, meta=(DisplayName="生命回复"))
	float HealthRegen;

	// 生命回复成长
	UPROPERTY(EditAnywhere, meta=(DisplayName="生命回复成长"))
	float HealthRegenGrowthRate;

	// 基础最大法力
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础最大法力"))
	float BaseMaxMana;
	
	// 法术回复
	UPROPERTY(EditAnywhere, meta=(DisplayName="法术回复"))
	float ManaRegen;

	// 法力回复成长
	UPROPERTY(EditAnywhere, meta=(DisplayName="法力回复成长"))
	float ManaRegenGrowthRate;

	// 基础护甲
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础护甲"))
	float BaseArmor;

	// 基础法术抗性
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础法术抗性"))
	float BaseMagicResistance;

	// 基础移动速度
	UPROPERTY(EditAnywhere, meta=(DisplayName="基础移动速度"))
	float BaseMoveSpeed;
};