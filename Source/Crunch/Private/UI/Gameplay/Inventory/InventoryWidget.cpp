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
