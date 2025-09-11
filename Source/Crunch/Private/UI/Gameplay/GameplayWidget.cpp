// 幻雨喜欢小猫咪


#include "UI/Gameplay/GameplayWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/Engine.h"
#include "GameplayMenu/GameplayMenu.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"
#include "UI/Gameplay/Chat/ChatMessageItemWidget.h"
#include "UI/Gameplay/StatusEffect/StatusEffectWidget.h"


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

		// 配置状态效果显示
		if (StatusEffectWidget)
		{
			StatusEffectWidget->ConfigureWithASC(OwnerAbilitySystemComponent);
		}
	}

	// 初始设置：隐藏鼠标，只允许游戏输入
	SetShowMouseCursor(false);
	SetFocusToGameOnly();
	
	// 绑定游戏菜单的继续按钮事件
	if (GameplayMenu)
	{
		GameplayMenu->GetResumeButtonClickedEventDelegate().AddDynamic(
			this, 
			&UGameplayWidget::ToggleGameplayMenu
		);
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

void UGameplayWidget::ToggleGameplayMenu()
{
	// 如果当前显示的是游戏菜单
	if (MainSwitcher->GetActiveWidget() == GameplayMenuRootPanel)
	{
		// 切换到游戏主界面
		MainSwitcher->SetActiveWidget(GameplayWidgetRootPanel);
		
		// 启用玩家输入
		SetOwningPawnInputEnabled(true);
		
		// 隐藏鼠标光标
		SetShowMouseCursor(false);
		
		// 切换到纯游戏输入模式
		SetFocusToGameOnly();
	}
	else // 如果当前显示的是游戏界面
	{
		// 显示游戏菜单
		ShowGameplayMenu();
	}
}

void UGameplayWidget::ShowGameplayMenu()
{
	// 切换到游戏菜单界面
	MainSwitcher->SetActiveWidget(GameplayMenuRootPanel);

	// 禁用玩家输入
	SetOwningPawnInputEnabled(false);

	// 显示鼠标光标
	SetShowMouseCursor(true);

	// 切换到游戏+UI输入模式
	SetFocusToGameAndUI();
}

void UGameplayWidget::SetGameplayMenuTitle(const FString& NewTitle)
{
	// 更新游戏菜单标题
	GameplayMenu->SetTitleText(NewTitle);
}

void UGameplayWidget::SmartToggleChat()
{
	if (ChatWidget)
	{
		// 如果处于临时模式，直接进入正常聊天模式
		if (ChatWidget->IsInTemporaryMode())
		{
			UE_LOG(LogTemp, Warning, TEXT("从临时模式进入正常聊天模式"));
			ChatWidget->ShowChatWidget(); // 直接显示聊天框
		}
		else
		{
			// 正常模式下进行切换
			ChatWidget->ToggleChatWidget();
		}
	}
}

void UGameplayWidget::ShowChat()
{
	if (ChatWidget)
	{
		ChatWidget->ShowChatWidget();
	}
}

void UGameplayWidget::HideChat()
{
	if (ChatWidget)
	{
		ChatWidget->HideChatWidget();
	}
}

void UGameplayWidget::ShowTemporaryChatMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
	UE_LOG(LogTemp, Warning, TEXT("GameplayWidget显示临时聊天消息：%s"), *Message.MessageContent);
	
	if (ChatWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("将临时消息传递给ChatWidget"));
		ChatWidget->ShowTemporaryMessage(Message, bIsSelf, bIsTeammate);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ChatWidget为空，无法显示临时聊天消息"));
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

void UGameplayWidget::ShowBarrageMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
	UE_LOG(LogTemp, Warning, TEXT("GameplayWidget显示弹幕消息：%s"), *Message.MessageContent);
	
	if (!GameplayWidgetRootPanel || !ChatMessageItemClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GameplayWidgetRootPanel或ChatMessageItemClass为空，无法显示弹幕"));
		return;
	}
	
	// 获取实际屏幕大小
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	if (ViewportSize.X <= 0 || ViewportSize.Y <= 0)
	{
		// 如果获取失败，使用默认值
		ViewportSize = FVector2D(1920.0f, 1080.0f);
		UE_LOG(LogTemp, Warning, TEXT("无法获取屏幕大小，使用默认值"));
	}
	
	// 创建弹幕消息项
	UChatMessageItemWidget* BarrageItem = CreateWidget<UChatMessageItemWidget>(this, ChatMessageItemClass);
	if (BarrageItem)
	{
		// 将弹幕添加到游戏主界面面板
		UCanvasPanelSlot* CanvasSlot = GameplayWidgetRootPanel->AddChildToCanvas(BarrageItem);
		if (CanvasSlot)
		{
			// 计算弹幕位置（居中区域 + 随机浮动）
			float CenterY = ViewportSize.Y * 0.5f; // 屏幕中心
			float FloatRange = ViewportSize.Y * 0.3f; // 上下浮动范围（屏幕高度的30%）
			
			// 使用随机数生成Y位置
			float RandomOffset = FMath::RandRange(-FloatRange * 0.5f, FloatRange * 0.5f);
			float BarrageY = CenterY + RandomOffset;
			
			// 确保弹幕不会超出屏幕边界
			float MinY = ViewportSize.Y * 0.1f; // 距离顶部10%
			float MaxY = ViewportSize.Y * 0.9f; // 距离底部10%
			BarrageY = FMath::Clamp(BarrageY, MinY, MaxY);
			
			// 设置初始位置（屏幕右侧外更远，符合弹幕风格）和大小
			float StartX = ViewportSize.X + 200.0f; // 屏幕外侧200像素，更靠右，符合弹幕风格
			CanvasSlot->SetPosition(FVector2D(StartX, BarrageY));
			CanvasSlot->SetSize(FVector2D(400.0f, 30.0f));
			
			// 设置为弹幕模式
			BarrageItem->SetAsBarrageMode(Message, bIsSelf, bIsTeammate, GameplayWidgetRootPanel);
			
			UE_LOG(LogTemp, Warning, TEXT("弹幕消息创建成功，屏幕大小：(%.2f, %.2f)，弹幕位置：(%.2f, %.2f)"), 
			       ViewportSize.X, ViewportSize.Y, StartX, BarrageY);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("弹幕消息项创建失败"));
	}
}