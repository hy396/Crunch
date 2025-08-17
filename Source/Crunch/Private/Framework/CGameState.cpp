// 幻雨喜欢小猫咪


#include "CGameState.h"

#include "Net/UnrealNetwork.h"

void ACGameState::RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot)
{
	// 仅服务器处理且目标槽位未被占用
	if (!HasAuthority() || IsSlotOccupied(DesiredSlot))
		return;

	// 查找当前玩家的已有选择
	FPlayerSelection* PlayerSelectionPtr =
		// 在 PlayerSelectionArray 数组中查找符合条件的元素
		PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
		{
			//对每个数组元素执行判断：是否匹配目标玩家
			return PlayerSelection.IsForPlayer(RequestingPlayer);
		}
	);

	if (PlayerSelectionPtr)
	{
		// 更新现有槽位
		PlayerSelectionPtr->SetSlot(DesiredSlot);
	}
	else
	{
		// 添加新的玩家选择
		PlayerSelectionArray.Add(FPlayerSelection(DesiredSlot, RequestingPlayer));
	}

	// 广播玩家选择更新
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}

bool ACGameState::IsSlotOccupied(uint8 SlotId) const
{
	// 寻找已经选择的玩家数组，查看是否有该插槽，如果找到说明给占了
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
	{
		if (PlayerSelection.GetPlayerSlot() == SlotId)
		{
			return true;
		}
	}

	return false;
}

const TArray<FPlayerSelection>& ACGameState::GetPlayerSelection() const
{
	return PlayerSelectionArray;
}

bool ACGameState::CanStartHeroSelection() const
{
	// 玩家数量和已选择的玩家数量相等
	return PlayerSelectionArray.Num() == PlayerArray.Num();
}

// bool ACGameState::CanStartMatch() const
// {
// 	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
// 	{
// 		if (PlayerSelection.GetCharacterDefination() == nullptr)
// 		{
// 			return false;
// 		}
// 	}
//
// 	return true;
// }

void ACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 将玩家选择数组声明为网络复制属性
	DOREPLIFETIME_CONDITION_NOTIFY(ACGameState, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
}

void ACGameState::OnRep_PlayerSelectionArray()
{
	// 广播玩家选择更新事件
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}
