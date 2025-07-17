// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ECC_AttackDamage.generated.h"

/**
 * 
 */
UCLASS()
class UECC_AttackDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UECC_AttackDamage();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
