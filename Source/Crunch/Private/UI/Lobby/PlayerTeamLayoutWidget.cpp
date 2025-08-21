// 幻雨喜欢小猫咪


#include "PlayerTeamLayoutWidget.h"

#include "PlayerTeamSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Network/TNetStatics.h"

void UPlayerTeamLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 清空现有布局
	TeamOneLayoutBox->ClearChildren();
	TeamTwoLayoutBox->ClearChildren();

	if (!PlayerTeamSlotWidgetClass) return;

	// 计算总玩家数量（每队人数 * 2）
	const int32 TotalPlayers = UTNetStatics::GetPlayerCountPerTeam() * 2;

	for (int32 i = 0; i < TotalPlayers; ++i)
	{
		// 创建新的槽位控件实例
		UPlayerTeamSlotWidget* NewSlotWidget = CreateWidget<UPlayerTeamSlotWidget>(GetOwningPlayer(), PlayerTeamSlotWidgetClass);
		// 添加到控件数组
		TeamSlotWidgets.Add(NewSlotWidget);
		// 队伍选择
		UHorizontalBoxSlot* NewSlot = i < UTNetStatics::GetPlayerCountPerTeam()
							? TeamOneLayoutBox->AddChildToHorizontalBox(NewSlotWidget)	// 添加到第一队
							: TeamTwoLayoutBox->AddChildToHorizontalBox(NewSlotWidget);	// 添加到第二队
		// 设置间距
		NewSlot->SetPadding(FMargin{PlayerTeamWidgetSlotMargin});
	}
}

void UPlayerTeamLayoutWidget::UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections)
{
	// 清空所有槽位
	for (UPlayerTeamSlotWidget* SlotWidget : TeamSlotWidgets)
	{
		SlotWidget->UpdateSlot("", nullptr);
	}
	// 遍历所有玩家选择
	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
			continue;

		TeamSlotWidgets[PlayerSelection.GetPlayerSlot()]->UpdateSlot(PlayerSelection.GetPlayerNickName(), PlayerSelection.GetCharacterDefinition());
	}
}
