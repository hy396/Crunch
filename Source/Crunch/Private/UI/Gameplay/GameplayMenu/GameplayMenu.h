// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "GameplayMenu.generated.h"

class UButton;
class UTextBlock;
class UComboBoxString;
class FOnButtonClickedEvent;
/**
 * 游戏内菜单控件
 * 提供继续游戏、返回主菜单、退出游戏等功能
 */
UCLASS()
class UGameplayMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构建时回调（初始化控件和事件绑定）
	virtual void NativeConstruct() override;

	// 获取继续游戏按钮的点击事件委托
	FOnButtonClickedEvent& GetResumeButtonClickedEventDelegate();

	// 设置菜单标题文本
	void SetTitleText(const FString& NewTitle);

// private:
	// 菜单标题文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> MenuTitle;

	// 继续游戏按钮控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ResumeBtn;

	// 返回主菜单按钮控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> MainMenuBtn;

	// 退出游戏按钮控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> QuitGameBtn;

	// 窗口模式选择下拉框
	// UPROPERTY(meta=(BindWidget))
	// TObjectPtr<UComboBoxString> WindowModeComboBox;

	// TODO:退出游戏有点抽象，不如返回菜单或返回房间
	// 返回主菜单回调
	UFUNCTION()
	void BackToMainMenu();

	// 退出游戏的回调
	UFUNCTION()
	void QuitGame();

	// // 窗口模式选择变化回调
	// UFUNCTION()
	// void OnWindowModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// // 初始化窗口模式选项
	// void InitializeWindowModeOptions();
	//
	// // 应用窗口模式设置
	// void ApplyWindowMode(const FString& WindowModeString);

	// 主菜单关卡引用
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;
};

