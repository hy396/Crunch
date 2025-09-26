// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ChatWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/CanvasPanel.h"
#include "ChatMessageItemWidget.generated.h"

/**
 * 聊天消息项控件
 * 此控件用于显示单条聊天消息，支持富文本格式和弹幕模式
 */
UCLASS()
class CRUNCH_API UChatMessageItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 设置聊天消息数据
	UFUNCTION(BlueprintCallable)
	void SetChatMessage(const FChatMessage& Message);

	// 设置聊天消息数据并指定颜色类型
	UFUNCTION(BlueprintCallable)
	void SetChatMessageWithColors(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

	// 设置为弹幕模式（使用弹幕动画）
	UFUNCTION(BlueprintCallable)
	void SetAsBarrageMode(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate, UCanvasPanel* BarragePanel);

	// 停止弹幕动画
	UFUNCTION(BlueprintCallable)
	void StopBarrageAnimation();

	// // 设置消息换行宽度
	// UFUNCTION(BlueprintCallable)
	// void SetMessageWrapWidth(bool bIsAuto);

	// 设置自动换行
	UFUNCTION(BlueprintCallable)
	void SetAutoWrapText(bool bAutoWrap);

private:
	// 消息富文本显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> MessageRichText;
	// // 尺寸框控件，用于控制消息尺寸和换行
	// UPROPERTY(meta=(BindWidget))
	// TObjectPtr<USizeBox> MessageSizeBox;
	
	// 处理消息内容中的特殊字符转义
	FString SanitizeMessageContent(const FString& Content) const;

	// 格式化带颜色的聊天消息文本
	FString FormatChatMessageWithColors(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate) const;

	// 获取频道类型颜色标签
	FString GetChannelColorTag(EChatChannelType ChannelType) const;

	// 获取发送者类型颜色标签
	FString GetSenderTypeColorTag(bool bIsSelf, bool bIsTeammate) const;

	// 弹幕相关变量
	// 弹幕消息持续时间（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Barrage")
	float BarrageMessageDuration = 8.0f;

	// 弹幕移动速度（像素/秒），针对30FPS优化
	UPROPERTY(EditDefaultsOnly, Category = "Barrage")
	float BarrageMoveSpeed = 250.0f;

	// 弹幕消息间的垂直间距
	UPROPERTY(EditDefaultsOnly, Category = "Barrage")
	float BarrageVerticalSpacing = 30.0f;

	// 弹幕状态变量
	bool bIsInBarrageMode = false;
	UPROPERTY()
	TObjectPtr<UCanvasPanel> OwningBarragePanel = nullptr;
	FTimerHandle BarrageTimerHandle;
	float BarrageStartTime = 0.0f;
	float InitialXPosition = 0.0f;

	// 弹幕移动更新方法
	UFUNCTION()
	void UpdateBarragePosition();

	// 弹幕结束回调
	UFUNCTION()
	void OnBarrageFinished();
};