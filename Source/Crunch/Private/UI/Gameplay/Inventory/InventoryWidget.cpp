// 幻雨喜欢小猫咪


#include "InventoryWidget.h"

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
				}
			}
		}
	}
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
	PopulatedItemEntryWidgets[DestinationWidget->GetItemHandle()] = SourceWidget;
	
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
