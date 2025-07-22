// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_LevelBased.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UMMC_LevelBased : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UMMC_LevelBased();

	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
private:
	// 属性成长值
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute RateAttribute;

	FGameplayEffectAttributeCaptureDefinition LevelCaptureDefinition;

	// FGameplayEffectAttributeCaptureDefinition RateCaptureDefinition;
};
