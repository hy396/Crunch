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

	// 设置玩家名称
	void SetPlayerNameFromPlayerState(APlayerState* PlayerState);

	bool GetPlayerNameIsSet() const{ return bPlayerNameSet; }
	
private:
	// 我方血条颜色
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor FriendlyColor;
	// 敌方血条颜色
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor HostileColor;

	// 是否显示玩家ID文本
	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bPlayerNameTextVisible = true;

	// 是否设置玩家名称成功
	bool bPlayerNameSet = false;

	// 玩家ID
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;
};
