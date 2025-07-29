// 幻雨喜欢小猫咪


#include "UI/Gameplay/GameplayWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"


void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		// 绑定属性回调
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());

		// 绑定每秒回复的属性
		HealthBar->SetRegenValueTextToGameplayAttribute(OwnerAbilitySystemComponent, UCHeroAttributeSet::GetHealthRegenAttribute());
		ManaBar->SetRegenValueTextToGameplayAttribute(OwnerAbilitySystemComponent, UCHeroAttributeSet::GetManaRegenAttribute());
	}
}

void UGameplayWidget::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	AbilityListView->ConfigureAbilities(Abilities);
}

void UGameplayWidget::ToggleShop()
{
	if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		// 显示商店界面
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
		
		// 播放弹出动画
		PlayShopPopupAnimation(true);
		
		// 禁用玩家输入
		SetOwningPawnInputEnabled(false);
		
		// 显示鼠标光标
		SetShowMouseCursor(true);
		
		// 切换到游戏+UI输入模式
		SetFocusToGameAndUI();
		
		// 设置焦点到商店控件
		ShopWidget->SetFocus();
	}
	else
	{
		// 隐藏商店界面
		ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		
		// 播放关闭动画
		PlayShopPopupAnimation(false);
		
		// 启用玩家输入
		SetOwningPawnInputEnabled(true);
		
		// 隐藏鼠标光标
		SetShowMouseCursor(false);
		
		// 切换到纯游戏输入模式
		SetFocusToGameOnly();
	}
}

void UGameplayWidget::PlayShopPopupAnimation(bool bPlayForward)
{
	if (bPlayForward)
	{
		// 正常播动画
		PlayAnimationForward(ShopPopupAnimation);
	}else
	{
		// 反着播动画
		PlayAnimationReverse(ShopPopupAnimation);
	}
}

void UGameplayWidget::SetOwningPawnInputEnabled(bool bPawnInputEnabled)
{
	if (bPawnInputEnabled)
	{
		// 启动玩家输入
		GetOwningPlayerPawn()->EnableInput(GetOwningPlayer());
	}else
	{
		// 禁用玩家输入
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}

void UGameplayWidget::SetShowMouseCursor(bool bShowMouseCursor)
{
	// 控制玩家控制器的鼠标显示
	GetOwningPlayer()->SetShowMouseCursor(bShowMouseCursor);
}

void UGameplayWidget::SetFocusToGameAndUI()
{
	// 创建游戏+UI输入模式
	FInputModeGameAndUI GameAndUIInputMode;
	
	// 设置捕获鼠标时不隐藏光标
	GameAndUIInputMode.SetHideCursorDuringCapture(false);
	
	// 应用输入模式
	GetOwningPlayer()->SetInputMode(GameAndUIInputMode);
	
}

void UGameplayWidget::SetFocusToGameOnly()
{
	// 创建纯游戏输入模式
	FInputModeGameOnly GameOnlyInputMode;
	
	// 应用输入模式
	GetOwningPlayer()->SetInputMode(GameOnlyInputMode);
}
