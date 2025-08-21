// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MGameInstance.generated.h"

/**
 * 自定义游戏实例类 - 管理游戏全局状态和在线服务
 * 功能：
 *   - 玩家登录和身份验证
 *   - 会话创建、搜索和加入
 *   - 服务器协调和匹配
 *   - 关卡管理和切换
 */
UCLASS()
class CRUNCH_API UMGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	// 开始匹配（服务器端调用）
	void StartMatch();
private:	
	// 主菜单关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	// 大厅关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	// 游戏关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;

	// 加载关卡并监听连接
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};
