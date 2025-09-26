// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "WaitingWidget.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/MGameInstance.h"
#include "MainMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class CRUNCH_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	/******************************/	
	/*           Main             */
	/******************************/	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MainSwitcher;

	// 游戏实例
	UPROPERTY()
	TObjectPtr<UMGameInstance> MGameInstance;
	
	// 切换到主界面
	void SwitchToMainWidget();
	// 主界面的根组件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MainWidgetRoot;
	/******************************/	
	/*           会话			  */
	/******************************/
public:
	// 会话条目小部件类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Session")
	TSubclassOf<class USessionEntryWidget> SessionEntryWidgetClass;
private:
	// 创建会话按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CreateSessionButton;

	// 输入房间(会话)的名称的文本框
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableText> NewSessionNameText;

	// 点击“创建会话”按钮时调用
	UFUNCTION()
	void CreateSessionBtnClicked();

	// 取消房间创建（如等待界面点击取消时触发）
	UFUNCTION()
	void CancelSessionCreation();

	// 房间(会话)名称输入框文字变更时调用（用于校验是否可创建）
	UFUNCTION()
	void NewSessionNameTextChanged(const FText& NewText);

	// 加入房间(会话)失败时调用
	void JoinSessionFailed();

	// 更新房间列表
	void UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults);

	// 房间列表容器
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> SessionScrollBox;

	// 加入房间(会话)按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> JoinSessionBtn;


	// 当前选中的会话条目ID
	FString CurrentSelectedSessionId = "";

	// 点击“加入房间(会话)”按钮时调用
	UFUNCTION()
	void JoinSessionBtnClicked();

	// 会话条目被选中时调用
	void SessionEntrySelected(const FString& SelectedEntryIdStr);

	/******************************/	
	/*           Login            */
	/******************************/
private:
	// 登录界面的根组件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> LoginWidgetRoot;

	// 登录按钮
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoginButton;

	// 登录按钮点击事件
	UFUNCTION()
	void OnLoginButtonClicked();
	
	/**
	 * 登录完成时的回调函数
	 * @param bWasSuccessful 是否登录成功
	 * @param PlayerNickname 登录成功时的玩家昵称
	 * @param ErrorMsg 登录失败时的错误信息
	 */
	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMsg);

	/******************************/	
	/*           等待			  */
	/******************************/
private:
	// 等待界面
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWaitingWidget> WaitingWidget;
	// 切换到等待界面
	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel = false);
};