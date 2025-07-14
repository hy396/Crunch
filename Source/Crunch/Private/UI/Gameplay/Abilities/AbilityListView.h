// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "UI/Gameplay/Abilities/AbilityGauge.h"
#include "AbilityListView.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityListView : public UListView
{
	GENERATED_BODY()
public:
	// 配置能力表
	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);
private:
	UPROPERTY(EditAnywhere, Category = "Data")
	TObjectPtr<UDataTable> AbilityDataTable;

	// 技能生成绑定
	void AbilityGaugeGenerated(UUserWidget& Widget);

	// 查找指定技能的UI数据
	const FAbilityWidgetData* FindWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const;
};
