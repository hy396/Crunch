// 幻雨喜欢小猫咪


#include "ChatMessageItemWidget.h"
#include "Components/RichTextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Engine.h"

void UChatMessageItemWidget::SetChatMessage(const FChatMessage& Message)
{
	// 使用带颜色的格式化方法，默认为不是自己也不是队友（即对手）
	SetChatMessageWithColors(Message, false, false);
}

void UChatMessageItemWidget::SetChatMessageWithColors(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
	if (!MessageRichText) return;

	FString FormattedMessage = FormatChatMessageWithColors(Message, bIsSelf, bIsTeammate);
	MessageRichText->SetText(FText::FromString(FormattedMessage));
	
	UE_LOG(LogTemp, Log, TEXT("设置聊天消息: %s"), *FormattedMessage);
}

FString UChatMessageItemWidget::GetChannelColorTag(EChatChannelType ChannelType) const
{
	switch (ChannelType)
	{
	case EChatChannelType::Team:
		return TEXT("RichTextBlock.TeamChatStyle");
	case EChatChannelType::All:
		return TEXT("RichTextBlock.AllChatStyle");
	default:
		return TEXT("RichTextBlock.DefaultStyle");
	}
}

FString UChatMessageItemWidget::FormatChatMessageWithColors(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate) const
{
	// 获取发送者类型颜色标签
	FString SenderColorTag = GetSenderTypeColorTag(bIsSelf, bIsTeammate);
	
	// 获取频道类型颜色标签
	FString ChannelColorTag = GetChannelColorTag(Message.ChannelType);
    
	// 格式化时间
	FString TimeString = Message.Timestamp.ToString(TEXT("%H:%M"));
    
	// 构建频道标识
	FString ChannelPrefix = (Message.ChannelType == EChatChannelType::All) ? TEXT("全体") : TEXT("队伍");
    
	// 构建发送者类型标识
	FString SenderPrefix = bIsSelf ? TEXT("我") :
		bIsTeammate ? TEXT("队友") : TEXT("对手");
    
	// 处理消息内容中的特殊字符
	FString SafeMessageContent = SanitizeMessageContent(Message.MessageContent);
    
	// 构建带颜色的消息格式：[时间] [频道] [发送者类型] 玩家名: 消息内容
	// 弹幕和正常聊天使用相同的格式，保持内容一致性
	// FString FormattedMessage = FString::Printf(
	// 	TEXT("<RichTextBlock.TimeStyle>[%s]</><%s>[%s]</><%s>[%s]%s:</><RichTextBlock.DefaultStyle>%s</>"),
	// 	*TimeString,
	// 	*ChannelColorTag,
	// 	*ChannelPrefix,
	// 	*SenderColorTag,
	// 	*SenderPrefix,
	// 	*Message.SenderName,
	// 	*SafeMessageContent
	// );
	FString FormattedMessage = FString::Printf(
		TEXT("<RichTextBlock.TimeStyle>[%s] </><%s>[%s] </><%s>[%s]%s:</><RichTextBlock.DefaultStyle>%s</>"),
		*TimeString,
		*ChannelColorTag,
		*ChannelPrefix,
		*SenderColorTag,
		*SenderPrefix,
		*Message.SenderName,
		*SafeMessageContent
	);


	return FormattedMessage;
}

FString UChatMessageItemWidget::GetSenderTypeColorTag(bool bIsSelf, bool bIsTeammate) const
{
	return bIsSelf ? TEXT("RichTextBlock.SelfChatStyle") :	// 自己的消息 - 绿色
		bIsTeammate ? TEXT("RichTextBlock.TeamChatStyle")	// 队友的消息 - 蓝色
		: TEXT("RichTextBlock.EnemyChatStyle");				// 对手的消息 - 红色

}

void UChatMessageItemWidget::SetAsBarrageMode(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate, UCanvasPanel* BarragePanel)
{
	if (!BarragePanel)
	{
		UE_LOG(LogTemp, Error, TEXT("BarragePanel为空，无法设置弹幕模式"));
		return;
	}

	// 记录弹幕状态
	bIsInBarrageMode = true;
	OwningBarragePanel = BarragePanel;
	BarrageStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// 设置消息内容（使用与正常聊天相同的格式）
	SetChatMessageWithColors(Message, bIsSelf, bIsTeammate);

	// 获取屏幕宽度作为初始位置
	if (UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this))
	{
		InitialXPosition = CanvasSlot->GetPosition().X;
	}

	// 启动定时器进行弹幕移动
	if (GetWorld())
	{
		// 使用较小的间隔来实现流畅的动画（30FPS）
		GetWorld()->GetTimerManager().SetTimer(
			BarrageTimerHandle,
			this,
			&UChatMessageItemWidget::UpdateBarragePosition,
			1.0f / 30.0f, // 30FPS
			true // 循环
		);

		// 设置弹幕结束定时器
		FTimerHandle FinishTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			FinishTimerHandle,
			this,
			&UChatMessageItemWidget::OnBarrageFinished,
			BarrageMessageDuration,
			false // 不循环
		);
	}

	UE_LOG(LogTemp, Warning, TEXT("弹幕模式设置成功，初始位置：%.2f"), InitialXPosition);
}

void UChatMessageItemWidget::UpdateBarragePosition()
{
	if (!bIsInBarrageMode || !GetWorld()) return;

	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(this);
	if (!CanvasSlot) return;

	// 计算当前应该的X位置
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float ElapsedTime = CurrentTime - BarrageStartTime;
	float NewX = InitialXPosition - (BarrageMoveSpeed * ElapsedTime);

	// 获取当前位置
	FVector2D CurrentPosition = CanvasSlot->GetPosition();
	
	// 更新X位置
	CanvasSlot->SetPosition(FVector2D(NewX, CurrentPosition.Y));

	// 如果移出屏幕左侧，提前结束
	if (NewX < -CanvasSlot->GetSize().X)
	{
		OnBarrageFinished();
	}
}

void UChatMessageItemWidget::OnBarrageFinished()
{
	UE_LOG(LogTemp, Log, TEXT("弹幕动画结束"));
	
	// 停止弹幕动画
	StopBarrageAnimation();
	
	// 从父控件中移除
	RemoveFromParent();
}

void UChatMessageItemWidget::StopBarrageAnimation()
{
	if (GetWorld() && BarrageTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BarrageTimerHandle);
		BarrageTimerHandle.Invalidate();
	}
	
	bIsInBarrageMode = false;
	OwningBarragePanel = nullptr;
}

// void UChatMessageItemWidget::SetMessageWrapWidth(bool bIsAuto)
// {
// 	if (MessageRichText)
// 	{
// 		MessageRichText->SetAutoWrapText(bIsAuto);
// 	}
// }

void UChatMessageItemWidget::SetAutoWrapText(bool bAutoWrap)
{
	if (MessageRichText)
	{
		MessageRichText->SetAutoWrapText(bAutoWrap);
	}
}

FString UChatMessageItemWidget::SanitizeMessageContent(const FString& Content) const
{
	// 处理消息内容中的特殊字符，避免富文本解析错误
	if (Content.Contains(TEXT("<")) || Content.Contains(TEXT(">"))) 
	{
		FString SafeContent = Content;
		SafeContent = SafeContent.Replace(TEXT("<"), TEXT("&lt;"));
		SafeContent = SafeContent.Replace(TEXT(">"), TEXT("&gt;"));
		
		UE_LOG(LogTemp, Warning, TEXT("消息内容包含特殊字符，已进行转义处理: %s"), *SafeContent);
		return SafeContent;
	}
	
	return Content;
}
