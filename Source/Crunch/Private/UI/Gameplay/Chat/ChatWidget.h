// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Framework/Application/SlateApplication.h"  // 添加这个头文件用于FSlateApplication
#include "ChatWidget.generated.h"

class IChatInterface;



UENUM(BlueprintType)
enum class EChatChannelType : uint8
{
	Team    UMETA(DisplayName = "队友聊天"),
	All     UMETA(DisplayName = "全体聊天")
};

USTRUCT(BlueprintType)
struct CRUNCH_API FChatMessage
{
	GENERATED_BODY()

	// 发送者名称
	UPROPERTY(BlueprintReadOnly)
	FString SenderName;

	// 消息内容
	UPROPERTY(BlueprintReadOnly)
	FString MessageContent;

	// 聊天频道类型
	UPROPERTY(BlueprintReadOnly)
	EChatChannelType ChannelType;

	// 发送者团队ID
	UPROPERTY(BlueprintReadOnly)
	FGenericTeamId SenderTeamId;

	// 时间戳
	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;

	FChatMessage()
	{
		SenderName = TEXT("");
		MessageContent = TEXT("");
		ChannelType = EChatChannelType::All;
		SenderTeamId = FGenericTeamId::NoTeam;
		Timestamp = FDateTime::Now();
	}

	FChatMessage(const FString& InSenderName, const FString& InMessage, EChatChannelType InChannelType, FGenericTeamId InTeamId)
		: SenderName(InSenderName)
		, MessageContent(InMessage)
		, ChannelType(InChannelType)
		, SenderTeamId(InTeamId)
		, Timestamp(FDateTime::Now())
	{
	}
};

/**
 * 
 */
UCLASS()
class CRUNCH_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构建时回调
	virtual void NativeConstruct() override;
	
	// 销毁时回调
	virtual void NativeDestruct() override;

	// 显示聊天界面
	UFUNCTION(BlueprintCallable)
	void ShowChatWidget();

	// 隐藏聊天界面
	UFUNCTION(BlueprintCallable)
	void HideChatWidget();

	// 切换聊天界面显示状态
	UFUNCTION(BlueprintCallable)
	void ToggleChatWidget();

	// 添加消息到聊天列表
	UFUNCTION(BlueprintCallable)
	void AddMessageToChat(const FChatMessage& Message, bool bIsSelf = false, bool bIsTeammate = false);

	// 显示临时消息弹窗（10秒后自动隐藏）
	UFUNCTION(BlueprintCallable)
	void ShowTemporaryMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

	// 发送聊天消息
	UFUNCTION(BlueprintCallable)
	void SendChatMessage();

	// 设置发送消息后是否自动隐藏聊天框
	UFUNCTION(BlueprintCallable)
	void SetAutoHideAfterSend(bool bAutoHide) { bAutoHideAfterSend = bAutoHide; }

	// 设置输入焦点到聊天输入框
	UFUNCTION(BlueprintCallable)
	void FocusChatInput();
	
	// 取消输入焦点
	UFUNCTION(BlueprintCallable)
	void UnfocusChatInput();

	// 检查是否处于临时消息模式
	UFUNCTION(BlueprintCallable)
	bool IsInTemporaryMode() const { return bIsTemporaryMode; }

	// 获取频道选择下拉框
	UComboBoxString* GetChannelComboBox() const { return ChannelComboBox; }
	
	// 设置聊天提示图标可见性
	// void SetChatHintVisibility(bool bVisible);  // 已移除ChatHintIcon，不再需要这个函数

protected:
	// 处理输入框的按键事件
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// 聊天消息滚动框
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	// 聊天消息区域的边框
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> ChatMessagesBorder;

	// 聊天输入框
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> ChatInputBox;

	// 发送按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SendButton;

	// 聊天频道选择下拉框
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> ChannelComboBox;

	// 聊天消息项控件类
	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	TSubclassOf<UUserWidget> ChatMessageItemClass;

	// 玩家控制器引用（支持游戏内和大厅两种控制器）
	UPROPERTY()
	TObjectPtr<APlayerController> OwningPlayerController;

	// 发送按钮点击事件
	UFUNCTION()
	void OnSendButtonClicked();

	// 输入框回车事件
	UFUNCTION()
	void OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// 获取当前选择的聊天频道
	EChatChannelType GetSelectedChannelType() const;

	// 初始化聊天频道选项
	void InitializeChatChannels();

	// 创建并添加消息项到聊天列表
	bool CreateAndAddMessageItem(const FChatMessage& Message, bool bIsSelf = false, bool bIsTeammate = false);
	
	// 清除临时消息定时器
	void ClearTemporaryMessageTimer();

	// 隐藏临时消息
	UFUNCTION()
	void HideTemporaryMessage();

	// 设置输入区域可见性
	void SetInputAreaVisibility(bool bVisible, bool bHandleFocus = true);
	
	// === 新增的统一方法 ===
	// 验证输入组件有效性
	bool IsInputValid() const;
	
	// 退出临时模式
	void ExitTemporaryMode();
	
	// 发送消息到服务器
	bool SendMessageToServer(const FString& MessageText, EChatChannelType ChannelType);
	
	// 处理发送后的行为
	void HandlePostSendBehavior();
	
	// 清理输入并恢复游戏模式
	void ClearInputAndRestoreGameMode();
	
	// // 消息的宽度
	// UPROPERTY(EditDefaultsOnly, Category = "Chat")
	// float MessageWidth = 500.0f;

	// 最大聊天消息数量
	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	int32 MaxChatMessages = 100;

	// 临时消息显示相关
	// 临时消息显示时间（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Popup")
	float TemporaryMessageDuration = 10.0f;

	// 临时消息隐藏定时器句柄
	FTimerHandle TemporaryMessageTimerHandle;

	// 临时消息模式标记
	bool bIsTemporaryMode = false;

	// 发送消息后是否自动隐藏聊天框（默认为true，适用于游戏内聊天）
	bool bAutoHideAfterSend = true;
	
	// 隐藏聊天消息区域
	void HideChatMessagesArea();
};
