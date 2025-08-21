// 幻雨喜欢小猫咪


#include "MGameInstance.h"

void UMGameInstance::StartMatch()
{
	// 检查是否为专用服务器或监听服务器, 在服务器中切地图
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		// 加载游戏关卡并监听
		LoadLevelAndListen(GameLevel);
	}
}

void UMGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	// 从软引用的 UWorld 获取包路径（比如 /Game/Maps/MyMap.MyMap）
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	if (LevelURL != "")
	{
		// 切换到指定关卡，并加上 "?listen" 参数
		GetWorld()->ServerTravel(LevelURL.ToString() + "?listen");
	}
}
