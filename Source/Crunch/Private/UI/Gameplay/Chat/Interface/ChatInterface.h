// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UI/Gameplay/Chat/ChatWidget.h"
#include "ChatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UChatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 聊天接口
 * 所有支持聊天功能的PlayerController都应该实现这个接口
 * 提供统一的聊天消息发送和接收接口
 */
class CRUNCH_API IChatInterface
{
	GENERATED_BODY()

public:
	/**
	 * 发送聊天消息到服务器
	 * @param Message 消息内容
	 * @param ChannelType 聊天频道类型
	 */
	virtual void SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType) = 0;

	/**
	 * 接收来自服务器的聊天消息
	 * @param Message 聊天消息
	 */
	virtual void ReceiveChatMessageFromServer(const FChatMessage& Message) = 0;
};