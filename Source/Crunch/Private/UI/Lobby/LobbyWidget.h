// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

class ACGameState;
class ALobbyPlayerController;
class UTeamSelectionWidget;
class UUniformGridPanel;
class UButton;
class UWidgetSwitcher;
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


	// 玩家控制器
	UPROPERTY()
	TObjectPtr<ALobbyPlayerController> LobbyPlayerController;
	
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
};
