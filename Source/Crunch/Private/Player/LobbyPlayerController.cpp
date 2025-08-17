// 幻雨喜欢小猫咪


#include "LobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Framework/CGameState.h"

void ALobbyPlayerController::Server_RequestSlotSelectionChange_Implementation(uint8 NewSlotID)
{
	if (!GetWorld())
		return;

	// 获取当前游戏状态
	ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
	if (!CGameState)
		return;

	// 委托给游戏状态管理器处理槽位变更
	CGameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotID);
} 

bool ALobbyPlayerController::Server_RequestSlotSelectionChange_Validate(uint8 NewSlotID)
{
	return true;
}
