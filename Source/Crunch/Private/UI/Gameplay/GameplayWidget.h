// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "StatsGauge.h"
#include "ValueGauge.h"
#include "Abilities/AbilityListView.h"
#include "Blueprint/UserWidget.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 当Widget被创建并添加到视口时，此函数会被自动调用
	virtual void NativeConstruct() override;

	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);
private:
	// 生命进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;

	// 法力进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

	// 技能列表
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;

	// 属性面板：攻击力显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> AttackPowerGauge;

	// 属性面板：法术强度显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> MagicPowerGauge;
	
	// 属性面板：护甲显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> ArmorGauge;
	
	// 属性面板：法术抗性显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> MagicResistanceGauge;

	// 属性面板：移动速度显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> MoveSpeedGauge;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};