// 幻雨喜欢小猫咪


#include "GameplayMenu.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Network/TNetStatics.h"

void UGameplayMenu::NativeConstruct()
{
	Super::NativeConstruct();
	MainMenuBtn->OnClicked.AddDynamic(this, &UGameplayMenu::BackToMainMenu);
	QuitGameBtn->OnClicked.AddDynamic(this, &UGameplayMenu::QuitGame);
	
	// // 初始化窗口模式选项
	// InitializeWindowModeOptions();
	//
	// // 绑定窗口模式选择变化事件
	// if (WindowModeComboBox)
	// {
	// 	WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UGameplayMenu::OnWindowModeSelectionChanged);
	// }
}

FOnButtonClickedEvent& UGameplayMenu::GetResumeButtonClickedEventDelegate()
{
	return ResumeBtn->OnClicked;
}

void UGameplayMenu::SetTitleText(const FString& NewTitle)
{
	MenuTitle->SetText(FText::FromString(NewTitle));
}

void UGameplayMenu::BackToMainMenu()
{
	// TODO:返回主菜单(已实现) 2025/11/25实现但暂时为实验，可能成功不知道是否有bug --- 测试完貌似没bug
	IOnlineSessionPtr SessionInterface = UTNetStatics::GetSessionPtr();
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(MainMenuLevel.ToString()));

	if (!SessionInterface.IsValid())
	{
		GetOwningPlayer()->ClientTravel(*LevelURL.ToString(), TRAVEL_Absolute);
		// GetWorld()->GetFirstPlayerController()->ClientTravel(*LevelURL.ToString(), TRAVEL_Absolute);
		return;
	}

	FString SessionName = UTNetStatics::GetSessionNameStr();

	// 如果本地没有这个会话，就直接返回主菜单
	if (SessionInterface->GetNamedSession(FName(*SessionName)) == nullptr)
	{
		GetOwningPlayer()->ClientTravel(*LevelURL.ToString(), TRAVEL_Absolute);
		// GetWorld()->GetFirstPlayerController()->ClientTravel(*LevelURL.ToString(), TRAVEL_Absolute);
		return;
	}

	// 🔥 绑定 DestroySession 完成回调
	// OnDestroySessionCompleteDelegateHandle =
	// 	SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
	// 		FOnDestroySessionCompleteDelegate::CreateUObject(
	// 			this, &ALobbyPlayerController::OnDestroySessionComplete
	// 		));
	// 移除旧回调，避免重复触发
	SessionInterface->OnDestroySessionCompleteDelegates.RemoveAll(this);
	SessionInterface->OnDestroySessionCompleteDelegates.AddLambda(
			[this, LevelURL](FName SessionNameQwQ, bool bWasSuccessful)
			{
				// const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(MainMenuLevel.ToString()));
					GetOwningPlayer()->ClientTravel(*LevelURL.ToString(), TRAVEL_Absolute);
			});
	// 🔥 必须 Destroy，不要 End
	SessionInterface->DestroySession(FName(SessionName));
}

void UGameplayMenu::QuitGame()
{
	// 退出游戏
	// UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

// 已经完成操作
// void UGameplayMenu::OnWindowModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
// {
// 	// 只在用户直接选择时应用设置，避免程序初始化时触发
// 	if (SelectionType == ESelectInfo::OnMouseClick || SelectionType == ESelectInfo::OnKeyPress)
// 	{
// 		ApplyWindowMode(SelectedItem);
// 	}
// }

// void UGameplayMenu::InitializeWindowModeOptions()
// {
// 	if (!WindowModeComboBox)
// 	{
// 		return;
// 	}
// 	
// 	// 清空现有选项
// 	WindowModeComboBox->ClearOptions();
// 	
// 	// 添加窗口模式选项
// 	WindowModeComboBox->AddOption(TEXT("全屏"));
// 	WindowModeComboBox->AddOption(TEXT("窗口全屏"));
// 	WindowModeComboBox->AddOption(TEXT("窗口模式"));
// 	
// 	// 获取当前窗口模式并设置默认选择
// 	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
// 	if (GameUserSettings)
// 	{
// 		EWindowMode::Type CurrentWindowMode = GameUserSettings->GetFullscreenMode();
// 		FString CurrentModeString;
// 		
// 		switch (CurrentWindowMode)
// 		{
// 		case EWindowMode::Fullscreen:
// 			CurrentModeString = TEXT("全屏");
// 			break;
// 		case EWindowMode::WindowedFullscreen:
// 			CurrentModeString = TEXT("窗口全屏");
// 			break;
// 		case EWindowMode::Windowed:
// 			CurrentModeString = TEXT("窗口模式");
// 			break;
// 		default:
// 			CurrentModeString = TEXT("窗口模式");
// 			break;
// 		}
// 		
// 		// 设置当前选择
// 		WindowModeComboBox->SetSelectedOption(CurrentModeString);
// 	}
// }
//
// void UGameplayMenu::ApplyWindowMode(const FString& WindowModeString)
// {
// 	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
// 	if (!GameUserSettings)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("[GameplayMenu] 无法获取GameUserSettings"));
// 		return;
// 	}
// 	
// 	EWindowMode::Type NewWindowMode;
// 	
// 	// 根据选择的字符串确定窗口模式
// 	if (WindowModeString == TEXT("全屏"))
// 	{
// 		NewWindowMode = EWindowMode::Fullscreen;
// 	}
// 	else if (WindowModeString == TEXT("窗口全屏"))
// 	{
// 		NewWindowMode = EWindowMode::WindowedFullscreen;
// 	}
// 	else if (WindowModeString == TEXT("窗口模式"))
// 	{
// 		NewWindowMode = EWindowMode::Windowed;
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("[GameplayMenu] 未知的窗口模式: %s"), *WindowModeString);
// 		return;
// 	}
// 	
// 	// 获取当前窗口模式，避免重复设置
// 	EWindowMode::Type CurrentWindowMode = GameUserSettings->GetFullscreenMode();
// 	if (CurrentWindowMode == NewWindowMode)
// 	{
// 		UE_LOG(LogTemp, Log, TEXT("[GameplayMenu] 窗口模式已经是: %s，跳过设置"), *WindowModeString);
// 		return;
// 	}
// 	
// 	// 如果切换到全屏模式，确保设置了合适的分辨率
// 	if (NewWindowMode == EWindowMode::Fullscreen)
// 	{
// 		// 获取桌面分辨率作为全屏分辨率
// 		FIntPoint DesktopResolution = GameUserSettings->GetDesktopResolution();
// 		GameUserSettings->SetScreenResolution(DesktopResolution);
// 		UE_LOG(LogTemp, Log, TEXT("[GameplayMenu] 设置全屏分辨率为: %dx%d"), DesktopResolution.X, DesktopResolution.Y);
// 	}
// 	
// 	// 应用窗口模式设置
// 	GameUserSettings->SetFullscreenMode(NewWindowMode);
// 	// GameUserSettings->ConfirmVideoMode();
// 	
// 	// 应用设置并检查基准性能
// 	GameUserSettings->ApplySettings(true);
// 	
// 	// 保存设置到配置文件
// 	GameUserSettings->SaveSettings();
// 	
// 	UE_LOG(LogTemp, Log, TEXT("[GameplayMenu] 成功应用窗口模式: %s"), *WindowModeString);
// }
