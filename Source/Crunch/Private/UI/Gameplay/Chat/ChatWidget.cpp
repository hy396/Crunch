// 幻雨喜欢小猫咪


#include "ChatWidget.h"
#include "ChatMessageItemWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interface/ChatInterface.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UChatWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 获取玩家控制器（支持任何实现IChatInterface的控制器）
    OwningPlayerController = GetOwningPlayer();

    // 绑定发送按钮事件
    if (SendButton)
    {
        SendButton->OnClicked.AddDynamic(this, &UChatWidget::OnSendButtonClicked);
    }

    // 绑定输入框回车事件
    if (ChatInputBox)
    {
        ChatInputBox->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatInputCommitted);
    }

    // 初始化聊天频道
    InitializeChatChannels();

    // 默认隐藏聊天界面
    SetVisibility(ESlateVisibility::Collapsed);
}

void UChatWidget::ShowChatWidget()
{
    // 退出临时模式（如果处于该模式）
    ExitTemporaryMode();
    
    // 显示输入区域
    SetInputAreaVisibility(true);
    
    // 显示聊天界面
    SetVisibility(ESlateVisibility::Visible);
    
    // 设置输入焦点
    FocusChatInput();
}

void UChatWidget::HideChatWidget()
{
    // 退出临时模式（如果处于该模式）
    ExitTemporaryMode();
    
    // 隐藏聊天界面
    SetVisibility(ESlateVisibility::Collapsed);
    
    // 清空输入框并恢复游戏输入模式
    ClearInputAndRestoreGameMode();
}

void UChatWidget::ToggleChatWidget()
{
    if (GetVisibility() == ESlateVisibility::Visible)
    {
        HideChatWidget();
    }
    else
    {
        ShowChatWidget();
    }
}

void UChatWidget::AddMessageToChat(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
    UE_LOG(LogTemp, Warning, TEXT("ChatWidget正在添加消息：%s"), *Message.MessageContent);
    
    // 使用统一的消息项创建方法
    if (CreateAndAddMessageItem(Message, bIsSelf, bIsTeammate))
    {
        UE_LOG(LogTemp, Warning, TEXT("消息添加成功"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("消息添加失败"));
    }
}

void UChatWidget::SendChatMessage()
{
    // 验证输入有效性
    if (!IsInputValid()) return;

    FString MessageText = ChatInputBox->GetText().ToString().TrimStartAndEnd();
    if (MessageText.IsEmpty()) return;

    // 获取频道类型并发送消息
    EChatChannelType ChannelType = GetSelectedChannelType();
    if (SendMessageToServer(MessageText, ChannelType))
    {
        // 清空输入框并处理发送后的行为
        HandlePostSendBehavior();
    }
}

void UChatWidget::FocusChatInput()
{
    if (!IsInputValid()) return;
    
    UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(), this);
    ChatInputBox->SetUserFocus(GetOwningPlayer());
}

FReply UChatWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // 按下ESC键关闭聊天界面
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
        HideChatWidget();
        return FReply::Handled();
    }
    
    // 按下Enter键发送消息
    if (InKeyEvent.GetKey() == EKeys::Enter)
    {
        SendChatMessage();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UChatWidget::OnSendButtonClicked()
{
    SendChatMessage();
}

void UChatWidget::OnChatInputCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        SendChatMessage();
    }
}

EChatChannelType UChatWidget::GetSelectedChannelType() const
{
    if (!ChannelComboBox) return EChatChannelType::All;

    static const FString TeamChatText = TEXT("队伍聊天");

    FString SelectedOption = ChannelComboBox->GetSelectedOption();
    if (SelectedOption == TeamChatText)
    {
        return EChatChannelType::Team;
    }
    return EChatChannelType::All;
}

void UChatWidget::InitializeChatChannels()
{
    if (!ChannelComboBox) return;

    // 使用常量避免硬编码
    static const FString AllChatText = TEXT("全体聊天");
    static const FString TeamChatText = TEXT("队伍聊天");

    ChannelComboBox->ClearOptions();
    ChannelComboBox->AddOption(AllChatText);
    ChannelComboBox->AddOption(TeamChatText);
    ChannelComboBox->SetSelectedOption(TeamChatText);
}

// 添加统一的输入验证方法
bool UChatWidget::IsInputValid() const
{
    return ChatInputBox && OwningPlayerController && GetOwningPlayer();
}

// 添加统一的临时模式退出方法
void UChatWidget::ExitTemporaryMode()
{
    if (bIsTemporaryMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("退出临时消息模式"));
        ClearTemporaryMessageTimer();
        bIsTemporaryMode = false;
    }
}

// 添加统一的消息发送方法
bool UChatWidget::SendMessageToServer(const FString& MessageText, EChatChannelType ChannelType)
{
    UE_LOG(LogTemp, Warning, TEXT("发送聊天消息：%s 频道：%d"), *MessageText, (int32)ChannelType);

    if (IChatInterface* ChatInterface = Cast<IChatInterface>(OwningPlayerController))
    {
        ChatInterface->SendChatMessageToServer(MessageText, ChannelType);
        UE_LOG(LogTemp, Log, TEXT("成功发送聊天消息，控制器类型：%s"), *OwningPlayerController->GetClass()->GetName());
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("控制器 %s 未实现 IChatInterface 接口"), *OwningPlayerController->GetClass()->GetName());
        return false;
    }
}

// 添加统一的发送后处理方法
void UChatWidget::HandlePostSendBehavior()
{
    // 清空输入框
    ChatInputBox->SetText(FText::GetEmpty());
    
    // 根据配置决定发送后的行为
    if (bAutoHideAfterSend)
    {
        HideChatWidget();
    }
    else
    {
        FocusChatInput();
    }
}

// 添加统一的输入清理和游戏模式恢复方法
void UChatWidget::ClearInputAndRestoreGameMode()
{
    if (ChatInputBox)
    {
        ChatInputBox->SetText(FText::GetEmpty());
    }
    
    if (GetOwningPlayer())
    {
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());
    }
}
bool UChatWidget::CreateAndAddMessageItem(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
    if (!ChatScrollBox || !ChatMessageItemClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ChatScrollBox或ChatMessageItemClass为空"));
        return false;
    }

    // 创建消息项控件
    UChatMessageItemWidget* MessageItem = CreateWidget<UChatMessageItemWidget>(this, ChatMessageItemClass);
    if (!MessageItem)
    {
        UE_LOG(LogTemp, Error, TEXT("消息项控件创建失败"));
        return false;
    }

    // 根据参数决定使用哪种方法设置消息
    if (bIsSelf || bIsTeammate)
    {
        MessageItem->SetChatMessageWithColors(Message, bIsSelf, bIsTeammate);
    }
    else
    {
        MessageItem->SetChatMessage(Message);
    }
    
    // 添加到滚动框并滚动到底部
    ChatScrollBox->AddChild(MessageItem);
    ChatScrollBox->ScrollToEnd();
    
    // 限制最大消息数量（仅在非临时模式下）
    if (!bIsTemporaryMode && ChatScrollBox->GetChildrenCount() > MaxChatMessages)
    {
        ChatScrollBox->RemoveChildAt(0);
    }
    
    return true;
}

// 添加统一的定时器清理方法
void UChatWidget::ClearTemporaryMessageTimer()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TemporaryMessageTimerHandle);
    }
}

void UChatWidget::ShowTemporaryMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
    UE_LOG(LogTemp, Warning, TEXT("显示临时消息：%s 发送者类型：自己=%s 队友=%s"), 
           *Message.MessageContent, bIsSelf ? TEXT("true") : TEXT("false"), bIsTeammate ? TEXT("true") : TEXT("false"));
    
    // 清除之前的定时器
    ClearTemporaryMessageTimer();
    
    // 设置为临时模式
    bIsTemporaryMode = true;
    
    // 隐藏输入区域
    SetInputAreaVisibility(false);
    
    // 使用统一的消息项创建方法
    if (CreateAndAddMessageItem(Message, bIsSelf, bIsTeammate))
    {
        UE_LOG(LogTemp, Warning, TEXT("临时消息项控件创建成功，添加到列表"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("临时消息项控件创建失败"));
        return;
    }
    
    // 显示聊天窗口
    SetVisibility(ESlateVisibility::Visible);
    
    // 设置定时器自动隐藏
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            TemporaryMessageTimerHandle,
            this,
            &UChatWidget::HideTemporaryMessage,
            TemporaryMessageDuration,
            false
        );
    }
}

void UChatWidget::HideTemporaryMessage()
{
    UE_LOG(LogTemp, Warning, TEXT("隐藏临时消息"));
    
    // 恢复正常模式
    bIsTemporaryMode = false;
    
    // 显示输入区域
    SetInputAreaVisibility(true);
    
    // 隐藏聊天窗口
    SetVisibility(ESlateVisibility::Collapsed);
    
    // 清除定时器
    ClearTemporaryMessageTimer();
}

void UChatWidget::SetInputAreaVisibility(bool bVisible)
{
    ESlateVisibility TargetVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
    
    if (ChatInputBox)
    {
        ChatInputBox->SetVisibility(TargetVisibility);
    }
    
    if (SendButton)
    {
        SendButton->SetVisibility(TargetVisibility);
    }
    
    if (ChannelComboBox)
    {
        ChannelComboBox->SetVisibility(TargetVisibility);
    }
}