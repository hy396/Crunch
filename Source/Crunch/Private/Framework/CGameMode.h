// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameModeBase.h"
#include "CGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ACGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

	// 重写：游戏开始时调用
	virtual void StartPlay() override;
private:
	// 获取指定玩家的队伍ID
	FGenericTeamId GetTeamIDForPlayer(const AController* InController) const;
	
	// 根据队伍ID查找下一个出生点
	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const;
	
	// 队伍ID到出生点Tag的映射
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TMap<FGenericTeamId, FName> TeamStartSpotTagMap;

	// 获取风暴核心对象
	class AStormCore* GetStormCore() const;

	// 比赛结束处理
	void MatchFinished(AActor* ViewTarget, int WiningTeam);
};
