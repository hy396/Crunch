// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "StatsGauge.h"
#include "ValueGauge.h"
#include "Abilities/AbilityListView.h"
#include "Blueprint/UserWidget.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "Inventory/InventoryWidget.h"
#include "Shop/ShopWidget.h"
#include "GameplayWidget.generated.h"

class UCrosshairWidget;
class UCanvasPanel;
class UWidgetSwitcher;
class UGameplayMenu;
class UMatchStatWidget;
class USkeletalMeshRenderWidget;
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
private:
	// 生命进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;

	// 法力进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

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
};