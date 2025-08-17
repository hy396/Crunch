// 幻雨喜欢小猫咪


#include "LobbyWidget.h"

#include "TeamSelectionWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Framework/CGameState.h"
#include "Network/TNetStatics.h"
#include "Player/LobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateTeamSelectionSlots();
	// 获取玩家控制器
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	// 配置游戏状态
	ConfigureGameState();
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	// TODO: 正常来说是这样操作
	// 生成两队玩家槽
	for (int i = 0; i < UTNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		// 创建槽位
		if (UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass))
		{
			// 设置槽ID
			NewSelectionSlot->SetSlotID(i);
			// 添加到网格布局
			if (UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot))
			{
				// 计算行列位置
				int Row = i % UTNetStatics::GetPlayerCountPerTeam();
				int Column = i < UTNetStatics::GetPlayerCountPerTeam() ? 0 : 1;

				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Column);
			}
			// 绑定槽点击事件产生的广播委托
			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ULobbyWidget::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID)
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestSlotSelectionChange(NewSlotID);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 获取游戏状态
	CGameState = World->GetGameState<ACGameState>();
	if (!CGameState)
	{
		// 如果没有找到，设置定时器定期重试
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ULobbyWidget::ConfigureGameState, 1.f);
	}else
	{
		// 绑定玩家选择更新事件
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ULobbyWidget::UpdatePlayerSelectionDisplay);
		// 初始化显示当前玩家选择
		UpdatePlayerSelectionDisplay(CGameState->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	// 清空所有槽位显示
	for (UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots)
	{
		SelectionSlot->UpdateSlotInfo("Empty");
	}
	// 更新每个玩家的槽位显示
	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
			continue;
		// 更新槽位名称显示
		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());
	}
}
