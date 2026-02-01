// 幻雨喜欢小猫咪


#include "CGameState.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Network/TNetStatics.h"
#include "Player/CPlayerCharacter.h"
#include "Player/CPlayerController.h"
#include "Player/MPlayerState.h"

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

void ACGameState::SetCharacterSelected(const APlayerState* SelectingPlayer,
	const UPDA_CharacterDefinition* SelectedDefinition)
{
	// 检查角色是否已被选择
	if (IsDefinitionSelected(SelectedDefinition)) return;

	// 查找玩家选择条目
	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.IsForPlayer(SelectingPlayer);
		}
	);

	if (FoundPlayerSelection)
	{
		// 更新角色定义
		FoundPlayerSelection->SetCharacterDefinition(SelectedDefinition);
		// 广播更新
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
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

bool ACGameState::IsDefinitionSelected(const UPDA_CharacterDefinition* Definition) const
{
	// 遍历玩家选择数组检查指定角色是否已被选择
	const FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetCharacterDefinition() == Definition;
		}
	);

	return FoundPlayerSelection != nullptr;
}

void ACGameState::SetCharacterDeselected(const UPDA_CharacterDefinition* DefinitionToDeselect)
{
	if (!DefinitionToDeselect) return;

	// 查找对应的角色选择条目
	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetCharacterDefinition() == DefinitionToDeselect;
		}
	);

	if (FoundPlayerSelection)
	{
		// 置空角色定义
		FoundPlayerSelection->SetCharacterDefinition(nullptr);
		// 广播更新
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
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

bool ACGameState::CanStartMatch() const
{
	// 遍历玩家选择数组，检查每个玩家是否已选择角色
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
	{
		if (PlayerSelection.GetCharacterDefinition() == nullptr)
		{
			return false;
		}
	}

	return true;
}

void ACGameState::AddTeamOnePlayerKillCount()
{
	if (!HasAuthority()) return;
	TeamOnePlayerKillCount++;
}

void ACGameState::AddTeamTwoPlayerKillCount()
{
	if (!HasAuthority()) return;
	TeamTwoPlayerKillCount++;
}

void ACGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 将玩家选择数组声明为网络复制属性
	DOREPLIFETIME(ACGameState, TeamOnePlayerKillCount);
	DOREPLIFETIME(ACGameState, TeamTwoPlayerKillCount);
	DOREPLIFETIME_CONDITION_NOTIFY(ACGameState, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
}

void ACGameState::OnRep_PlayerSelectionArray()
{
	// 广播玩家选择更新事件
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}

bool ACGameState::AutoAssignEmptySlot(const APlayerState* NewPlayerState)
{
    if (!NewPlayerState) return false;
	// 查找当前玩家的已有选择
	FPlayerSelection* PlayerSelectionPtr =
		// 在 PlayerSelectionArray 数组中查找符合条件的元素
		PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
		{
			//对每个数组元素执行判断：是否匹配目标玩家
			return PlayerSelection.IsForPlayer(NewPlayerState);
		}
	);
	// 如果以及有了选择则退出
	if (PlayerSelectionPtr)
	{
		return true;
	}
    // 查找第一个空槽位
    const int32 TotalSlots = UTNetStatics::GetPlayerCountPerTeam() * 2;
    for (int32 SlotId = 0; SlotId < TotalSlots; ++SlotId)
    {
        if (!IsSlotOccupied(SlotId))
        {
            // 分配空槽位
            PlayerSelectionArray.Add(FPlayerSelection(SlotId, NewPlayerState));
            OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
            UE_LOG(LogTemp, Warning, TEXT("[服务器] 为玩家 %s 自动分配槽位 %d"), 
                   *NewPlayerState->GetPlayerName(), SlotId);
            return true;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[服务器] 无法为玩家 %s 分配槽位：服务器已满"), 
           *NewPlayerState->GetPlayerName());
    return false;
}

void ACGameState::RemovePlayerSelection(const FUniqueNetIdRepl& LeavingPlayerId)
{
	// 查找并移除离开玩家的选择记录
    int32 RemovedCount = PlayerSelectionArray.RemoveAll([&](const FPlayerSelection& PlayerSelection)
    {
        return PlayerSelection.GetPLayerUniqueId() == LeavingPlayerId;
    });
    
    if (RemovedCount > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[服务器] 移除了 %d 个离开玩家的选择记录"), RemovedCount);
        // 广播玩家选择更新
        OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
    }
}

void ACGameState::OnRep_TeamOnePlayerKillCount()
{
	if (ACPlayerController* PC = Cast<ACPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->UpdateTeamOnePlayerKillCount(TeamOnePlayerKillCount);
	}
}

void ACGameState::OnRep_TeamTwoPlayerKillCount()
{
	if (ACPlayerController* PC = Cast<ACPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->UpdateTeamTwoPlayerKillCount(TeamTwoPlayerKillCount);
	}
}

// void ACGameState::Server_NotifyPlayerKilled_Implementation(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates)
// {
// 	// 在服务器上广播击杀事件
// 	// OnPlayerKilled.Broadcast(KillerState, VictimState, AssistStates);
//
// 	// 发送到所有客户端
// 	Client_OnPlayerKilled(KillerState, VictimState, AssistStates);
// }
//
// bool ACGameState::Server_NotifyPlayerKilled_Validate(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates)
// {
// 	return true;
// }
// void ACGameState::Server_NotifyPlayerKilled(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates)
// {
// 	// 在服务器上广播击杀事件
// 	// OnPlayerKilled.Broadcast(KillerState, VictimState, AssistStates);
//
// 	// 发送到所有客户端
// 	Client_OnPlayerKilled(KillerState, VictimState, AssistStates);
// }
// void ACGameState::Client_OnPlayerKilled_Implementation(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates)
// {
// 	// 在客户端上广播击杀事件
// 	OnPlayerKilled.Broadcast(KillerState, VictimState, AssistStates);
// }

void ACGameState::Multicast_OnPlayerKilled_Implementation(AMPlayerState* KillerState, AMPlayerState* VictimState,
	const TArray<AMPlayerState*>& AssistStates)
{
	// 在客户端上广播击杀事件
	OnPlayerKilled.Broadcast(KillerState, VictimState, AssistStates);
}