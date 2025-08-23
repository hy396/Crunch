// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "TGameSession.generated.h"

/**
 * 自定义的游戏会话类，继承自 AGameSession
 * 主要用于管理玩家的注册、注销，以及自动登录等逻辑。
 */
UCLASS()
class ATGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:	
	// 处理自动登录逻辑，返回 true 表示成功
	virtual bool ProcessAutoLogin() override;
	
	// 当新玩家加入时调用
	// 负责将玩家注册进 Session，并通知 GameInstance
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;

	// 当玩家离开时调用
	// 负责将玩家从 Session 注销，并通知 GameInstance
	virtual void UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId) override;
};
