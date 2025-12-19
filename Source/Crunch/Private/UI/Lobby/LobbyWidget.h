// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Gameplay/Abilities/AbilityListView.h"
#include "Player/PlayerInfoTypes.h"
#include "UI/Gameplay/Chat/ChatWidget.h"
#include "Components/CanvasPanel.h"
#include "LobbyWidget.generated.h"

class UPlayerTeamLayoutWidget;
// class UAbilityListView;
class ACharacterDisplay;
class AMPlayerState;
class UTileView;
class ACGameState;
class ALobbyPlayerController;
class UTeamSelectionWidget;
class UUniformGridPanel;
class UButton;
class UWidgetSwitcher;
class UChatWidget;
/**
 * 
 */
UCLASS()
class CRUNCH_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
private:
	// 主界面切换器
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> MainSwitcher;
	// 队伍选择根节点
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UWidget> TeamSelectionRoot;
	// 开始英雄选择按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> StartHeroSelectionButton;
	// 队伍选择格子面板
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> TeamSelectionSlotGridPanel;

	// 队伍选择格子
	UPROPERTY(EditDefaultsOnly, Category = "TeamSelection")
	TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;

	// 所有队伍选择格子
	UPROPERTY()
	TArray<UTeamSelectionWidget*> TeamSelectionSlots;
	
	/**
	 * 清空并重新生成队伍选择槽位
	 * 根据玩家数量生成双队列网格布局
	 */
	void ClearAndPopulateTeamSelectionSlots();
	/**
	 * 处理槽位点击事件
	 * @param NewSlotID 新选择的槽位ID
	 */
	void SlotSelected(uint8 NewSlotID);

	// 英雄选择根节点（用于显示弹幕）
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UCanvasPanel> HeroSelectionRoot;

	// 角色选择列表
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UTileView> CharacterSelectionTileView;

	// 技能列表
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;

	// 玩家队伍布局
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPlayerTeamLayoutWidget> PlayerTeamLayoutWidget;

	// TODO:把开始按钮改为英雄选择确定按钮，添加一个定时器，倒数计时，全部英雄确定后，过一定时间开始游戏，如果有玩家没有选择英雄，自动选择一个英雄帮他确定
	// 开始游戏按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> StartMatchButton;
	
	// 退出大厅按钮
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> LeaveLobbyButton;
	
	// 玩家控制器
	UPROPERTY()
	TObjectPtr<ALobbyPlayerController> LobbyPlayerController;

	// 该大厅UI拥有者的玩家状态
	UPROPERTY()
	TObjectPtr<AMPlayerState> MPlayerState;
	
	/**
	 * 配置游戏状态监听
	 * 尝试获取游戏状态对象并绑定更新事件
	 */
	void ConfigureGameState();
	
	// 配置游戏状态定时器句柄
	FTimerHandle ConfigureGameStateTimerHandle;

	// 游戏状态
	UPROPERTY()
	TObjectPtr<ACGameState> CGameState;

	/**
	 * 更新玩家选择显示
	 * @param PlayerSelections 玩家选择信息数组
	 */
	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);

	// 开始英雄选择按钮点击事件处理
	UFUNCTION()
	void StartHeroSelectionButtonClicked();
	// 切换到英雄选择界面
	void SwitchToHeroSelection();
	// 角色定义加载完成回调，设置角色选择列表项
	void CharacterDefinitionLoaded();
	// 角色选择列表项点击事件处理
	void CharacterSelected(UObject* SelectedUObject);
	// 展示的角色类
	UPROPERTY(EditDefaultsOnly, Category = "Character Display")
	TSubclassOf<ACharacterDisplay> CharacterDisplayClass;
	// 存储用来展示的角色
	UPROPERTY()
	TObjectPtr<ACharacterDisplay> CharacterDisplay;
	// 创建角色展示
	void SpawnCharacterDisplay();
	// 更新角色展示
	void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection);

	// 启动游戏按钮点击事件处理
	UFUNCTION()
	void StartMatchButtonClicked();

	// 退出大厅按钮点击事件处理
	UFUNCTION()
	void LeaveLobbyButtonClicked();

private:
	// 聊天消息项控件类（用于弹幕）
	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	TSubclassOf<UUserWidget> ChatMessageItemClass;

	// 聊天组件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UChatWidget> ChatWidget;
public:
	// 聊天相关函数
	// 设置聊天输入焦点（用于回车键直接输入）
	UFUNCTION(BlueprintCallable)
	void FocusChatInput();

	// 显示弹幕消息（大厅弹幕效果）
	UFUNCTION(BlueprintCallable)
	void ShowBarrageMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

	UChatWidget* GetChatWidget(){ return ChatWidget;};
private:
	// 配置大厅模式的聊天频道（移除全体聊天选项）
	void ConfigureLobbyModeChat();
};
