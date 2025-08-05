// 幻雨喜欢小猫咪


#include "InventoryWidget.h"

#include "InventoryContextMenuWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/WrapBoxSlot.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		// 获取背包组件
		InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (InventoryComponent)
		{
			// 购买物品事件绑定
			InventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded);
			// 背包物品堆叠数量改变事件绑定
			InventoryComponent->OnItemStackCountChanged.AddUObject(this, &UInventoryWidget::ItemStackCountChanged);
			// 移除物品事件绑定
			InventoryComponent->OnItemRemoved.AddUObject(this, &UInventoryWidget::ItemRemoved);
			// 背包物品技能释放委托绑定
			InventoryComponent->OnItemAbilityCommitted.AddUObject(this, &UInventoryWidget::ItemAbilityCommitted);
            
			// 获取背包容量
			int32 Capacity = InventoryComponent->GetCapacity();
			// 清空背包
			ItemList->ClearChildren();
			ItemWidgets.Empty();
			for (int32 i = 0; i < Capacity; ++i)
			{
				UInventoryItemWidget* NewEmptyWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
				if (NewEmptyWidget)
				{
					NewEmptyWidget->SetSlotNumber(i);		//设置槽位编号
					// 添加到WarpBox
					UWrapBoxSlot* NewItemSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
					NewItemSlot->SetPadding(FMargin(2.f));	// 间隔
					ItemWidgets.Add(NewEmptyWidget);		// 添加到控件数组

					// 绑定拖拽放置事件
					NewEmptyWidget->OnInventoryItemDropped.AddUObject(this, &UInventoryWidget::HandleItemDragDrop);
					// 绑定左键点击事件，点击时调用 InventoryComponent 的 TryActivateItem（尝试使用物品）
					NewEmptyWidget->OnLeftButtonClicked.AddUObject(
						InventoryComponent, &UInventoryComponent::TryActivateItem
					);
					// 绑定右键点击事件，点击时在此 Widget 中切换显示上下文菜单
					NewEmptyWidget->OnRightButtonClicked.AddUObject(
						this, &UInventoryWidget::ToggleContextMenu
					);
				}
			}
			// 在界面中生成一个上下文菜单（初始状态为隐藏）
			SpawnContextMenu();
		}
	}
}

void UInventoryWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	// 如果焦点不在上下文菜单上,关闭菜单(使用和售出的按钮)
	if (!NewWidgetPath.ContainsWidget(ContextMenuWidget->GetCachedWidget().Get()))
	{
		ClearContextMenu();
	}
}

void UInventoryWidget::SpawnContextMenu()
{
	if (!ContextMenuWidgetClass) return;
	
	ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(this, ContextMenuWidgetClass);
	if (ContextMenuWidget)
	{
		// 绑定使用按钮和售出按钮
		ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &UInventoryWidget::UseFocusedItem);
		ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &UInventoryWidget::SellFocusedItem);

		// 将上下文菜单添加到视口，设置层级为 1，以确保浮于其他 UI 之上
		ContextMenuWidget->AddToViewport(1);
		// 初始时将上下文菜单隐藏
		SetContextMenuVisible(false);
	}
}

void UInventoryWidget::SellFocusedItem()
{
	// 通知库存组件出售物品
	InventoryComponent->SellItem(CurrentFocusedItemHandle);
	SetContextMenuVisible(false); // 关闭菜单
}

void UInventoryWidget::UseFocusedItem()
{
	// 通知库存组件激活物品
	InventoryComponent->TryActivateItem(CurrentFocusedItemHandle);
	SetContextMenuVisible(false); // 关闭菜单
}

void UInventoryWidget::SetContextMenuVisible(bool bContextMenuVisible)
{
	if (ContextMenuWidget)
	{
		ContextMenuWidget->SetVisibility(
			bContextMenuVisible ? 
			ESlateVisibility::Visible : 
			ESlateVisibility::Hidden
		);
	}
}

void UInventoryWidget::ToggleContextMenu(const FInventoryItemHandle& ItemHandle)
{
	// 如果点击的是当前焦点物品，则关闭菜单
	if (CurrentFocusedItemHandle == ItemHandle)
	{
		ClearContextMenu();
		return;
	}

	// 设置新的焦点物品
	CurrentFocusedItemHandle = ItemHandle;
	
	// 查找对应的物品控件
	TObjectPtr<UInventoryItemWidget>* ItemWidgetPtrPtr = PopulatedItemEntryWidgets.Find(ItemHandle);
	if (!ItemWidgetPtrPtr) return;
	
	UInventoryItemWidget* ItemWidget = *ItemWidgetPtrPtr;
	if (!ItemWidget) return;

	// 显示菜单
	SetContextMenuVisible(true);

	// 计算菜单位置（物品右侧中心点）
	FVector2D ItemAbsPos = ItemWidget->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D{1.f, 0.5f});

	// 转换为视口坐标
	FVector2D ItemWidgetPixelPos, ItemWidgetViewportPos;
	// 将绝对屏幕坐标转换为视口坐标系中的位置。
	USlateBlueprintLibrary::AbsoluteToViewport(this, ItemAbsPos, ItemWidgetPixelPos, ItemWidgetViewportPos);

	// 获取玩家控制器，以便查询视口尺寸，防止菜单被拉出屏幕
	if (APlayerController* OwningPlayerController = GetOwningPlayer())
	{
		int32 ViewportSizeX, ViewportSizeY;
		OwningPlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
		// 获取当前 UI 缩放比例（DPI 缩放）
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

		// 计算菜单底部是否超出屏幕下缘：
		// Overshoot = (菜单顶部Y坐标 + 菜单高度 * 缩放) - 屏幕高度
		float MenuHeightScaled = ContextMenuWidget->GetDesiredSize().Y * Scale;
		float Overshoot = ItemWidgetPixelPos.Y + MenuHeightScaled - ViewportSizeY;
		
		// 如果超出，则将菜单向上移动 Overshoot 像素，保证完全可见
		if (Overshoot > 0.f)
		{
			ItemWidgetPixelPos.Y -= Overshoot;
		}
	}

	// 设置菜单位置
	ContextMenuWidget->SetPositionInViewport(ItemWidgetPixelPos);
}

void UInventoryWidget::ClearContextMenu()
{
	SetContextMenuVisible(false);
	CurrentFocusedItemHandle = FInventoryItemHandle::InvalidHandle(); // 重置焦点物品
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
	if (!InventoryItem) return;

	// 获取下一个可用槽位
	if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
	{
		// 添加物品,将改槽位的显示刷新
		NextAvailableSlot->UpdateInventoryItem(InventoryItem);
		// 将槽位放入map
		PopulatedItemEntryWidgets.Add(InventoryItem->GetHandle(), NextAvailableSlot);

		// 通知库存组件槽位变化
		if (InventoryComponent)
		{
			InventoryComponent->ItemSlotChanged(
				InventoryItem->GetHandle(), 
				NextAvailableSlot->GetSlotNumber()
			);
		}
	}
}

void UInventoryWidget::ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
	// 查找对应物品并更新显示
	TObjectPtr<UInventoryItemWidget>* FoundWidget = PopulatedItemEntryWidgets.Find(Handle);
	if (FoundWidget)
	{
		(*FoundWidget)->UpdateStackCount();
	}
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	// 遍历寻找空位
	for (UInventoryItemWidget* ItemWidget: ItemWidgets)
	{
		if (ItemWidget->IsEmpty())
		{
			return ItemWidget;
		}
	}
	// 没有空位返回空
	return nullptr;
}

void UInventoryWidget::HandleItemDragDrop(
	UInventoryItemWidget* DestinationWidget,
	UInventoryItemWidget* SourceWidget)
{
	// 获取来源和目标的物品
	const UInventoryItem* SourceItem = SourceWidget->GetInventoryItem();
	const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem();

	// 交换显示的物品
	DestinationWidget->UpdateInventoryItem(SourceItem);
	SourceWidget->UpdateInventoryItem(DestinationItem);

	// 物品交换位置后,更新Map
	PopulatedItemEntryWidgets[DestinationWidget->GetItemHandle()] = DestinationWidget;
	
	// 通知库存组件目标槽位变化
	if (InventoryComponent)
	{
		InventoryComponent->ItemSlotChanged(
			DestinationWidget->GetItemHandle(), 
			DestinationWidget->GetSlotNumber()
		);
	}

	// 处理源槽位（如果非空）
	/**
	 * 其实就是目标插槽位置，但是两个在前面进行了显示的交换，
	 * 于是乎，目标插槽的位置身份变成了源插槽
	 * 这里就是看拖拽物品原本放进去的地方是否有东西，有的话需要刷新一下（因为我当时莫名的蒙了，就多写了点解释）
	 */
	if (!SourceWidget->IsEmpty())
	{
		// 更新Map
		PopulatedItemEntryWidgets[SourceWidget->GetItemHandle()] = SourceWidget;
		// 更新库存组件中的插槽
		if (InventoryComponent)
		{
			InventoryComponent->ItemSlotChanged(
				SourceWidget->GetItemHandle(), 
				SourceWidget->GetSlotNumber()
			);
		}
	}
}

void UInventoryWidget::ItemRemoved(const FInventoryItemHandle& ItemHandle)
{
	// 查找对应的物品
	if (TObjectPtr<UInventoryItemWidget>* FoundWidget = PopulatedItemEntryWidgets.Find(ItemHandle))
	{
		if (*FoundWidget)
		{
			// 清空槽位显示
			(*FoundWidget)->EmptySlot();
			
			// 从映射表中移除
			PopulatedItemEntryWidgets.Remove(ItemHandle);
		}
	}
}

void UInventoryWidget::ItemAbilityCommitted(const FInventoryItemHandle& ItemHandle, float CooldownDuration,
	float CooldownTimeRemaining)
{
	// 查找对应控件并启动冷却显示
	if (TObjectPtr<UInventoryItemWidget>* FoundWidget = PopulatedItemEntryWidgets.Find(ItemHandle))
	{
		if (*FoundWidget)
		{
			(*FoundWidget)->StartCooldown(CooldownDuration, CooldownTimeRemaining);
		}
	}
}
