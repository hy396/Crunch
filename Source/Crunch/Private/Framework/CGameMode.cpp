// 幻雨喜欢小猫咪


#include "Crunch/Private/Framework/CGameMode.h"

#include "EngineUtils.h"
#include "StormCore.h"
#include "GameFramework/PlayerStart.h"
#include "Player/CPlayerController.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	// 获取团队ID
	FGenericTeamId TeamId = GetTeamIDForPlayer(NewPlayerController);
	if (NewPlayerTeamInterface)
	{
		// 设置团队ID
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}

	// 分配出生点
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamId);
	return NewPlayerController;
}

void ACGameMode::StartPlay()
{
	Super::StartPlay();
	// 游戏开始时候，绑定风暴核心中的比赛结束的委托
	AStormCore* StormCore = GetStormCore();
	if (StormCore)
	{
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ACGameMode::MatchFinished);
	}
}

AStormCore* ACGameMode::GetStormCore() const
{
	// 寻找场景中的风暴核心
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<AStormCore> It(World); It; ++It)
		{
			return *It;
		}
	}

	return nullptr;
}

void ACGameMode::MatchFinished(AActor* ViewTarget, int WiningTeam)
{
	// 遍历场景中的所有玩家控制器，并调用MatchFinished方法
	UWorld* World = GetWorld();
	if (World)
	{
		for (TActorIterator<ACPlayerController> It(World); It; ++It)
		{
			It->MatchFinished(ViewTarget, WiningTeam);
		}
	}
}

FGenericTeamId ACGameMode::GetTeamIDForPlayer(const AController* InController) const
{
	// 没有玩家状态时，简单轮流分配队伍
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	// 在队伍出生点标签映射中查找该队伍对应的出生点标签
	const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
	if (!StartSpotTag)
	{
		return nullptr;
	}
	
	// 获取当前世界对象，用于遍历其中的演员
	UWorld *World = GetWorld();
	// 遍历世界中的所有APlayerStart演员，寻找匹配的重生点
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		// 检查当前重生点的标签是否与所需标签匹配
		if (It->PlayerStartTag == *StartSpotTag)
		{
			// 标记占用
			It->PlayerStartTag = FName("Taken");
			// 返回找到的重生点
			return *It;
		}
	}
	// 如果没有找到合适的重生点，返回nullptr
	return nullptr;
}
