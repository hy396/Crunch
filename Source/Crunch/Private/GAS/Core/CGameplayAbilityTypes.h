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

