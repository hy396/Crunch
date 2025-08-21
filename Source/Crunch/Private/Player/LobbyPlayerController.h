// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 玩家切换到英雄选择界面的委托声明
 * 当玩家控制器决定切换到英雄选择界面时触发
 */
DECLARE_DELEGATE(FOnSwitchToHeroSelection);

/**
 * 大厅玩家控制器类
 * 处理游戏大厅阶段的玩家输入和网络请求
 * 负责协调玩家槽位选择、英雄选择和比赛启动流程
 */
UCLASS()
class CRUNCH_API ALobbyPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()
public:
	ALobbyPlayerController();

	/**
	 * 切换到英雄选择界面的委托实例
	 * 当服务器确认可以开始英雄选择时触发
	 */
	FOnSwitchToHeroSelection OnSwitchToHeroSelection;
	
	/**
	 * 服务器端处理槽位选择变更请求
	 * @param NewSlotID 新的槽位ID
	 * 
	 * 网络调用：客户端请求变更槽位选择时调用
	 * 服务器验证后更新玩家槽位
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSlotSelectionChange(uint8 NewSlotID);

	/**
	 * 服务器端处理英雄选择开始请求
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartHeroSelection();

	/**
	 * 服务器端处理开始比赛请求
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartMatch();
	
	/**
	 * 客户端启动英雄选择流程
	 * 
	 * 网络调用：服务器确认后触发客户端切换界面
	 * 所有客户端收到调用后开始英雄选择
	 */
	UFUNCTION(Client, Reliable)
	void Client_StartHeroSelection();
};
