// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ValueGauge.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
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

	// 设置血条颜色
	void SetHealthBarColor(ETeamAttitude::Type TargetTeam);
private:
	// BarColor我方血条颜色
	// 敌方血条颜色
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor HostileColor;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;
};
