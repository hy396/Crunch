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

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

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
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;

	// 力量
	UPROPERTY(EditAnywhere)
	float Strength;

	// 智力
	UPROPERTY(EditAnywhere)
	float Intelligence;
    
	// 力量成长率
	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate;

	// 智力成长率
	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate;

	// 基础最大生命
	UPROPERTY(EditAnywhere)
	float BaseMaxHealth;

	// 基础最大法力
	UPROPERTY(EditAnywhere)
	float BaseMaxMana;

	// 基础攻击力
	UPROPERTY(EditAnywhere)
	float BaseAttackDamage;

	// 基础护甲
	UPROPERTY(EditAnywhere)
	float BaseArmor;

	// 基础移动速度
	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed;
};