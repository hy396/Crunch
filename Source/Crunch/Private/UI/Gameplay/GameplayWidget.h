// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "StatsGauge.h"
#include "ValueGauge.h"
#include "Abilities/AbilityListView.h"
#include "Blueprint/UserWidget.h"
#include "Chat/ChatWidget.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "Gauge/ValueBar.h"
#include "Inventory/InventoryWidget.h"
#include "Shop/ShopWidget.h"
#include "GameplayWidget.generated.h"

class UCrosshairWidget;
class UCanvasPanel;
class UWidgetSwitcher;
class UGameplayMenu;
class UMatchStatWidget;
class USkeletalMeshRenderWidget;
class UStatusEffectWidget;

/**
 * 
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 当Widget被创建并添加到视口时，此函数会被自动调用
	virtual void NativeConstruct() override;

	void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);

	// 切换商店的显示和隐藏
	void ToggleShop();

	// 切换游戏菜单显示/隐藏
	UFUNCTION()
	void ToggleGameplayMenu();

	// 显示游戏菜单（强制显示）
	void ShowGameplayMenu();

	// 设置游戏菜单标题文本
	void SetGameplayMenuTitle(const FString& NewTitle);

	// 聊天系统相关函数
	// 智能切换聊天：在临时模式下直接进入正常聊天，否则正常切换
	void SmartToggleChat();

	// 显示聊天窗口
	void ShowChat();

	// 隐藏聊天窗口
	void HideChat();

	// 显示临时聊天消息
	void ShowTemporaryChatMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

	// 显示弹幕消息
	void ShowBarrageMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

	// // 接收聊天消息（添加到聊天记录）
	// void ReceiveChatMessage(const FChatMessage& Message);

	
private:
	/**
	 * TODO: 将GameplayWidget中的生命条以及法力条修改为Lyra的版本（尚未完成）
	 * 预计创建一个新的进度条UI用于替代
	 */
	// 生命进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueBar> HealthValueBar;

	// 法力进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueBar> ManaValueBar;
	
	// 生命进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;

	// 法力进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

	// 状态效果显示控件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusEffectWidget> StatusEffectWidget;

	// 技能列表
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;

	// 属性面板：攻击力显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> AttackPowerGauge;

	// 属性面板：法术强度显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> MagicPowerGauge;
	
	// 属性面板：护甲显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> ArmorGauge;
	
	// 属性面板：法术抗性显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> MagicResistanceGauge;

	// 属性面板：移动速度显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStatsGauge> MoveSpeedGauge;

	// 商店
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UShopWidget> ShopWidget;

	// 背包UI
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	// 头像UI
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USkeletalMeshRenderWidget> HeadshotWidget;

	// 比赛统计信息控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMatchStatWidget> MatchStatWidget;

	// 游戏菜单
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGameplayMenu> GameplayMenu;
	
	// 主界面切换控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> MainSwitcher; 

	// 游戏主界面根面板
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> GameplayWidgetRootPanel; 

	// 游戏菜单根面板
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> GameplayMenuRootPanel;

	// 准星UI
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCrosshairWidget> CrosshairWidget;

	// 聊天UI
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UChatWidget> ChatWidget;
	
	// 商店弹出动画
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShopPopupAnimation;

	// 播放商店弹出动画
	void PlayShopPopupAnimation(bool bPlayForward);
	// 锁定或解锁玩家输入
	void SetOwningPawnInputEnabled(bool bPawnInputEnabled);
	// 设置鼠标光标显示状态
	void SetShowMouseCursor(bool bShowMouseCursor);
	// 设置游戏+UI输入模式
	void SetFocusToGameAndUI();
	// 设置纯游戏输入模式
	void SetFocusToGameOnly();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	// 聊天消息项控件类
	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	TSubclassOf<UUserWidget> ChatMessageItemClass;
};