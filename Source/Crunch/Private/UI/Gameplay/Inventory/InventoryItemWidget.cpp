// 幻雨喜欢小猫咪


#include "InventoryItemWidget.h"

#include "InventoryItemDragDropOp.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
	return !InventoryItem || !(InventoryItem->IsValid());
}

void UInventoryItemWidget::SetSlotNumber(int NewSlotNumber)
{
	SlotNumber = NewSlotNumber;
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
	// 清理之前的施法状态委托
	UnBindCanCastAbilityDelegate();
	
	InventoryItem = Item;
	// 如果物品无效或数量为0，清空槽位
	if (!InventoryItem || !InventoryItem->IsValid() || InventoryItem->GetStackCount() == 0)
	{
		EmptySlot();
		return;
	}
	// 设置图标
	SetIcon(InventoryItem->GetShopItem()->GetIcon());
	// 创建提示信息
	UItemToolTip* ToolTip = SetToolTipWidget(InventoryItem->GetShopItem());
	if (ToolTip)
	{
		ToolTip->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
	}

	// 处理可堆叠物品的显示逻辑
	if (InventoryItem->GetShopItem()->GetIsStackable())
	{
		StackCountText->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCount();
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}
	// 重置冷却显示
	ClearCooldown();
	// 检测物品是否有能力
	if (InventoryItem->IsGrantingAnyAbility())
	{
		// TODO:这里刚购买物品的时候会产生获取技能等级失败之类的警告，这是正常且必要的
		// 更新施法状态（是否可施放）
		UpdateCanCastDisplay(InventoryItem->CanCastAbility());
		
		// 获取冷却信息
		float AbilityCooldownRemaining = InventoryItem->GetAbilityCooldownTimeRemaining();
		float AbilityCooldownDuration = InventoryItem->GetAbilityCooldownDuration();
		// 如果还在冷却中，显示冷却
		if (AbilityCooldownRemaining > 0.f)
		{
			StartCooldown(AbilityCooldownDuration, AbilityCooldownRemaining);
		}

		// TODO:等着被我删吧，法力显示
		// 更新法力消耗显示
		float AbilityCost = InventoryItem->GetAbilityManaCost();
		ManaCostText->SetVisibility(AbilityCost == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		ManaCostText->SetText(FText::AsNumber(AbilityCost));

		// 更新总冷却时间显示
		CooldownDurationText->SetVisibility(AbilityCooldownDuration == 0.f? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		CooldownDurationText->SetText(FText::AsNumber(AbilityCooldownDuration));
		
		// 绑定施法状态变化委托
		BindCanCastAbilityDelegate();
	}
	else // 非能力物品
	{
		UpdateCanCastDisplay(true); // 总是显示为可施放状态
		ManaCostText->SetVisibility(ESlateVisibility::Hidden);
		CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
		CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryItemWidget::EmptySlot()
{
	ClearCooldown(); // 清除冷却显示
	UnBindCanCastAbilityDelegate(); // 解绑委托
	
	// 清空物品
	InventoryItem = nullptr;
	SetIcon(EmptyTexture);
	SetToolTip(nullptr);

	// 隐藏所有相关文本组件
	StackCountText->SetVisibility(ESlateVisibility::Hidden);
	ManaCostText->SetVisibility(ESlateVisibility::Hidden);
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownDurationText->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::UpdateStackCount()
{
	if (InventoryItem)
	{
		// 将堆叠数量转换为文本显示
		StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
	}
}

UTexture2D* UInventoryItemWidget::GetIconTexture()
{
	// 获取UI关联的商店物品
	if (InventoryItem && InventoryItem->GetShopItem())
	{
		// 商店物品中获取图标
		return InventoryItem->GetShopItem()->GetIcon();
	}
	return nullptr;
}

FInventoryItemHandle UInventoryItemWidget::GetItemHandle() const
{
	if (!IsEmpty())
	{
		// 获取物品句柄
		return InventoryItem->GetHandle();
	}
	// 插槽为空,返回无效句柄
	return FInventoryItemHandle::InvalidHandle();
}

void UInventoryItemWidget::UpdateCanCastDisplay(bool bCanCast)
{
	// 设置动态材质参数（1=可施放，0=不可施放）
	GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(
		CanCastDynamicMaterialParamName, 
		bCanCast ? 1.f : 0.f
	);
}

void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
		OnRightButtonClicked.Broadcast(GetItemHandle()); // 广播右键点击事件
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
		OnLeftButtonClicked.Broadcast(GetItemHandle()); // 广播左键点击事件
}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// 检查插槽是否有东西并且是否有拖拽类
	if (!IsEmpty() && DragDropOpClass)
	{
		UInventoryItemDragDropOp* DragDropOp = NewObject<UInventoryItemDragDropOp>(this,DragDropOpClass);
		if (DragDropOp)
		{
			DragDropOp->SetDraggedItem(this);	// 设置拖拽物品
			// 设置拖拽操作
			OutOperation = DragDropOp;
		}
	}
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	// 获取拖拽控件
	if (UInventoryItemWidget* OtherWidget = Cast<UInventoryItemWidget>(InOperation->Payload))
	{
		if (OtherWidget && !OtherWidget->IsEmpty())
		{
			// 广播物品放置事件（目标控件，来源控件）
			OnInventoryItemDropped.Broadcast(this, OtherWidget);
			return true; // 返回true表示处理成功
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemWidget::StartCooldown(float CooldownDuration, float TimeRemaining)
{
	// 设置冷却
	CooldownTimeRemaining = TimeRemaining;
	CooldownTimeDuration = CooldownDuration;

	// 万一冷却设置的时间弄错了,就不搞
	if (CooldownTimeRemaining > 0)
	{
		// TODO:感觉可以加一个UI动画，播放的时候先闪一下，不然倒数计时很突兀，CooldownTimeRemaining的时候也可以加一个结束的闪
		// 设置冷却结束定时器
		GetWorld()->GetTimerManager().SetTimer(
			CooldownDurationTimerHandle,
			this,
			&UInventoryItemWidget::CooldownFinished,
			CooldownTimeRemaining
			);
		// 设置冷却更新定时器（间隔更新）
		GetWorld()->GetTimerManager().SetTimer(
			CooldownUpdateTimerHandle, 
			this, 
			&UInventoryItemWidget::UpdateCooldown, 
			CooldownUpdateInterval, 
			true
			);
		// 先修改再显示，不然好抽象啊
		CooldownDisplayFormattingOptions.MaximumFractionalDigits = CooldownTimeRemaining > 1.f ? 0 : 2;
		CooldownCountText->SetText(FText::AsNumber(CooldownTimeRemaining, &CooldownDisplayFormattingOptions));
		// // 显示之前再刷新一下这个图标吧，不然也好抽象啊
		// if (GetItemIcon())
		// {
		// 	// 使用动态材质设置冷却进度
		// 	GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(
		// 		CooldownAmtDynamicMaterialParamName, 
		// 		1.f
		// 	);
		// }
		// 显示冷却倒计时文本
		CooldownCountText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventoryItemWidget::BindCanCastAbilityDelegate()
{
	if (InventoryItem)
	{
		// 通过const_cast移除const修饰以绑定委托
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.AddUObject(
			this, 
			&UInventoryItemWidget::UpdateCanCastDisplay
		);
	}
}

void UInventoryItemWidget::UnBindCanCastAbilityDelegate()
{
	if (InventoryItem)
	{
		// 移除所有与当前对象相关的委托绑定
		const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.RemoveAll(this);
	}
}

void UInventoryItemWidget::CooldownFinished()
{
	// 清除更新定时器
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);
	
	// 隐藏冷却倒计时文本
	CooldownCountText->SetVisibility(ESlateVisibility::Hidden);

	// 刷新材质
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(
			CooldownAmtDynamicMaterialParamName, 
			1.f
		);
	}
}

void UInventoryItemWidget::UpdateCooldown()
{
	// 更新剩余时间
	CooldownTimeRemaining -= CooldownUpdateInterval;
	
	// 计算冷却进度（0-1）
	float CooldownAmt = 1.f - CooldownTimeRemaining / CooldownTimeDuration;
	
	// 设置文本格式（大于1秒显示整数，小于1秒显示小数）
	CooldownDisplayFormattingOptions.MaximumFractionalDigits = CooldownTimeRemaining > 1.f ? 0 : 2;
	CooldownCountText->SetText(FText::AsNumber(CooldownTimeRemaining, &CooldownDisplayFormattingOptions));

	if (GetItemIcon())
	{
		// 使用动态材质设置冷却进度
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(
			CooldownAmtDynamicMaterialParamName, 
			CooldownAmt
		);
	}
}

void UInventoryItemWidget::ClearCooldown()
{
	CooldownFinished(); // 复用冷却结束逻辑
}

void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{
	if (GetItemIcon())
	{
		// 使用动态材质设置图标
		GetItemIcon()->GetDynamicMaterial()->SetTextureParameterValue(
			IconTextureDynamicMaterialParamName, 
			IconTexture
		);
		return;
	}
	Super::SetIcon(IconTexture);
}
