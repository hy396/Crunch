// 幻雨喜欢小猫咪


#include "MainMenuWidget.h"

#include "Components/Button.h"

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
	}

	// 绑定登录按钮点击事件
	LoginButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	// 绑定创建会话按钮点击事件
	CreateSessionButton->OnClicked.AddDynamic(this, &UMainMenuWidget::CreateSessionBtnClicked);
	// 绑定新会话名称输入框内容改变事件
	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMainMenuWidget::NewSessionNameTextChanged);
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

void UMainMenuWidget::OnLoginButtonClicked()
{
	// 登录按钮点击时触发
	UE_LOG(LogTemp, Warning, TEXT("Longing In!"))
	if (MGameInstance && !MGameInstance->IsLoggedIn() && !MGameInstance->IsLoggingIn())
	{
		// 触发登录
		MGameInstance->ClientAccountPortalLogin();
		// SwitchToWaitingWidget(FText::FromString("正在登录..."));
		SwitchToWaitingWidget(FText::FromString(FString(TEXT("正在登录..."))));
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
