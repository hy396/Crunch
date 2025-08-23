// 幻雨喜欢小猫咪


#include "Network/TGameSession.h"

#include "Framework/MGameInstance.h"


bool ATGameSession::ProcessAutoLogin()
{
	// return Super::ProcessAutoLogin();
	// 服务器不需要登录直接返回true
	return true;
}

void ATGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	// 获取游戏实例
	if (UMGameInstance* GameInstance = GetGameInstance<UMGameInstance>())
	{
		// 玩家加入(传入唯一ID)
		GameInstance->PlayerJoined(UniqueId);
	}
}

void ATGameSession::UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(FromSessionName, UniqueId);

	if (UMGameInstance* GameInstance = GetGameInstance<UMGameInstance>())
	{
		// 玩家退出(传入唯一ID)
		GameInstance->PlayerLeft(UniqueId);
	}
}
