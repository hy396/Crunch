// 幻雨喜欢小猫咪


#include "AbilityListView.h"

#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	// 绑定技能生成事件
	OnEntryWidgetGenerated().AddUObject(this, &UAbilityListView::AbilityGaugeGenerated);
	// 添加技能
	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	// 将控件转换为UAbilityGauge类型
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);

	if (AbilityGauge)
	{
		// 查找并配置技能计量器的数据
		AbilityGauge->ConfigureWithWidgetData(FindWidgetDataForAbility(AbilityGauge->GetListItem<UGameplayAbility>()->GetClass()));
	}
}

const FAbilityWidgetData* UAbilityListView::FindWidgetDataForAbility(
	const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	// 如果数据表为空则直接返回nullptr
	if (!AbilityDataTable) return nullptr;

	// 遍历数据表的所有行
	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		// 查找当前行的数据
		const FAbilityWidgetData* WidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityWidgetDataPair.Key, "");
        
		// 如果找到数据且技能类匹配则返回
		if (WidgetData && WidgetData->AbilityClass == AbilityClass)
		{
			return WidgetData;
		}
	}
    
	// 未找到匹配的数据
	return nullptr;
}