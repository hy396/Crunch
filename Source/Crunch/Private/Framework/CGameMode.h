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
	ACGameMode();
	
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

	// 游戏开始时调用
	virtual void StartPlay() override;

	// 获取控制器对应的默认Pawn类（优先玩家选择，否则用备用Pawn）
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* Controller) override;

	// 为玩家生成默认Pawn，并分配队伍与出生点
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

private:
	// 获取指定玩家的队伍ID
	FGenericTeamId GetTeamIDForPlayer(const AController* InController) const;
	
	// 根据队伍ID查找下一个出生点
	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const;

	// 玩家备份Pawn
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TSubclassOf<APawn> BackupPawn;
	
	// 队伍ID到出生点Tag的映射
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TMap<FGenericTeamId, FName> TeamStartSpotTagMap;

	// 获取风暴核心对象
	class AStormCore* GetStormCore() const;

	// 比赛结束处理
	void MatchFinished(AActor* ViewTarget, int WiningTeam);
};
