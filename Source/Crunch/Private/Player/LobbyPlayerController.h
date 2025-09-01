// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "UI/Gameplay/Chat/Interface/ChatInterface.h"	// 聊天接口
#include "UI/Gameplay/Chat/ChatWidget.h"
#include "UI/Lobby/LobbyWidget.h"
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
class CRUNCH_API ALobbyPlayerController : public AMenuPlayerController, public IChatInterface
{
	GENERATED_BODY()
public:
	ALobbyPlayerController();

	// 输入组件初始化
	virtual void SetupInputComponent() override;

	// UI组件初始化
	virtual void BeginPlay() override;

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

	// 聊天系统相关函数
	/**
	 * 服务器端处理聊天消息发送请求
	 * @param Message 消息内容
	 * @param ChannelType 聊天频道类型
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);

	/**
	 * 客户端接收聊天消息
	 * @param Message 聊天消息
	 */
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatMessage(const FChatMessage& Message);

	// IChatInterface 接口实现
	virtual void SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType) override;
	virtual void ReceiveChatMessageFromServer(const FChatMessage& Message) override;

private:
	// 缓存的大厅UI组件
	UPROPERTY()
	TObjectPtr<ULobbyWidget> CachedLobbyWidget;

	// 获取大厅UI组件
	ULobbyWidget* GetLobbyWidget();
};
