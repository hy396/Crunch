// 幻雨喜欢小猫咪


#include "MainMenuWidget.h"

#include "Components/Button.h"
#include "SessionEntryWidget.h"
#include "Network/TNetStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 获取游戏实例
	MGameInstance = GetGameInstance<UMGameInstance>();
	if (MGameInstance)
	{
		MGameInstance->OnLoginCompleted.AddUObject(this, &UMainMenuWidget::LoginCompleted);
		if (MGameInstance->IsLoggedIn())
		{
			SwitchToMainWidget();
		}
		// 绑定加入会话失败事件
		MGameInstance->OnJoinSessionFailed.AddUObject(this, &UMainMenuWidget::JoinSessionFailed);
		// 绑定会话搜索完成事件, 会话列表更新
		MGameInstance->OnGlobalSessionSearchCompleted.AddUObject(this, &UMainMenuWidget::UpdateLobbyList);
		// 开始全局会话搜索
		MGameInstance->StartGlobalSessionSearch();
	}

	// 绑定登录按钮点击事件
	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	// 绑定创建会话按钮点击事件
	CreateSessionButton->OnClicked.AddDynamic(this, &UMainMenuWidget::CreateSessionBtnClicked);
	// 绑定新会话名称输入框内容改变事件
	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMainMenuWidget::NewSessionNameTextChanged);
	// 绑定加入会话按钮点击事件
	JoinSessionBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::JoinSessionBtnClicked);
	// 设置加入按钮为不可用
	JoinSessionBtn->SetIsEnabled(false);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
}

void UMainMenuWidget::CreateSessionBtnClicked()
{
	// 确保玩家已登录
	if (MGameInstance && MGameInstance->IsLoggedIn())
	{
		// 请求创建并加入一个新的房间（会话）
		MGameInstance->RequestCreateAndJoinSession(FName(NewSessionNameText->GetText().ToString()));

		// 切换到等待界面，提示“Creating Lobby”，并绑定取消操作
		SwitchToWaitingWidget(FText::FromString(FString(TEXT("创建大厅"))), true)
			.AddDynamic(this, &UMainMenuWidget::CancelSessionCreation);
	}
}

void UMainMenuWidget::CancelSessionCreation()
{
	if (MGameInstance)
	{
		MGameInstance->CancelSessionCreation();
	}
	SwitchToMainWidget();
}

void UMainMenuWidget::NewSessionNameTextChanged(const FText& NewText)
{
	// 创建按钮是否可用
	CreateSessionButton->SetIsEnabled(!NewText.IsEmpty());
}

void UMainMenuWidget::JoinSessionFailed()
{
	// 加入会话失败,返回到主界面
	SwitchToMainWidget();
}

void UMainMenuWidget::UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("更新会话列表"))

	// 清理列表，重新加载
	SessionScrollBox->ClearChildren();

	bool bCurrentSelectedSessionValid = false;

	for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
	{
		// 创建会话条目
		USessionEntryWidget* NewSessionWidget = CreateWidget<USessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (NewSessionWidget)
		{
			// 获取会话名称
			FString SessionName = "Name_None";
			SearchResult.Session.SessionSettings.Get<FString>(UTNetStatics::GetSessionNameKey(), SessionName);

			// 获取会话ID
			FString SessionIdStr = SearchResult.Session.GetSessionIdStr();
			// 初始化会话条目,绑定按钮点击事件
			NewSessionWidget->InitializeEntry(SessionName, SessionIdStr);
			NewSessionWidget->OnSessionEntrySelected.AddUObject(this, &UMainMenuWidget::SessionEntrySelected);
			SessionScrollBox->AddChild(NewSessionWidget);

			// 检查之前选中的会话 ID 是否仍然存在
			if (CurrentSelectedSessionId == SessionIdStr)
			{
				bCurrentSelectedSessionValid = true;
			}
		}
	}
	// 如果之前的选择的会话无效了,则清空
	CurrentSelectedSessionId = bCurrentSelectedSessionValid ? CurrentSelectedSessionId : "";

	// 更新“加入会话”按钮是否可用
	JoinSessionBtn->SetIsEnabled(bCurrentSelectedSessionValid);	
}

void UMainMenuWidget::JoinSessionBtnClicked()
{
	// 检查是否有选中的会话
	if (MGameInstance && !CurrentSelectedSessionId.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[尝试加入会话] -> ID: %s"), *CurrentSelectedSessionId)
		// 尝试加入会话
		if (MGameInstance->JoinSessionWithId(CurrentSelectedSessionId))
		{
			SwitchToWaitingWidget(FText::FromString(FString(TEXT("正在加入房间"))));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[无法加入会话] -> 原因: 没有选中会话"))
	}
}

void UMainMenuWidget::SessionEntrySelected(const FString& SelectedEntryIdStr)
{
	CurrentSelectedSessionId = SelectedEntryIdStr;
}

void UMainMenuWidget::OnLoginButtonClicked()
{
	// 登录按钮点击时触发
	UE_LOG(LogTemp, Warning, TEXT("正在登录!"))
	if (MGameInstance && !MGameInstance->IsLoggedIn() && !MGameInstance->IsLoggingIn())
	{
		// 触发登录
		MGameInstance->ClientAccountPortalLogin();
		// SwitchToWaitingWidget(FText::FromString("正在登录"));
		SwitchToWaitingWidget(FText::FromString(FString(TEXT("正在登录"))));
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("登录成功: %s"), *PlayerNickname)
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("登录失败: %s"), *ErrorMsg)
	}
	SwitchToMainWidget();
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel)
{
	// 切换到等待界面
	MainSwitcher->SetActiveWidget(WaitingWidget);
	// 设置等待信息
	WaitingWidget->SetWaitInfo(WaitInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}
