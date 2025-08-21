// 幻雨喜欢小猫咪


#include "LobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Framework/CGameState.h"
#include "Framework/MGameInstance.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	// 设置bAutoManageActiveCameraTarget为false,表示不自动管理摄像机目标（由大厅逻辑手动控制）
	bAutoManageActiveCameraTarget = false;
}

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

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;

	// 遍历所有玩家控制器
	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		// 
		ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(*PlayerControllerIterator);
		if (PlayerController)
		{
			PlayerController->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	// 获取当前游戏实例
	if (UMGameInstance* CGameInstance = GetGameInstance<UMGameInstance>())
	{
		// 启动比赛流程
		CGameInstance->StartMatch();
	}
}

bool ALobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	// 触发界面切换委托
	OnSwitchToHeroSelection.ExecuteIfBound();
}


