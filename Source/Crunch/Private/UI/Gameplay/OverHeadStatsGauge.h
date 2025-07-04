// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ValueGauge.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "OverHeadStatsGauge.generated.h"

/**
 * 
 */
UCLASS()
class UOverHeadStatsGauge : public UUserWidget
{
	GENERATED_BODY()
public:
	
	void ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;
};
