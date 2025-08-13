// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AbilityCooldown.generated.h"

/**
 * 用于计算冷却的MMC计算类
 */
UCLASS()
class UMMC_AbilityCooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_AbilityCooldown();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
private:
	// FGameplayEffectAttributeCaptureDefinition DamageCaptureDef;
	// FGameplayEffectAttributeCaptureDefinition ArmorCaptureDef;
	FGameplayEffectAttributeCaptureDefinition CooldownReductionCaptureDef;
};
