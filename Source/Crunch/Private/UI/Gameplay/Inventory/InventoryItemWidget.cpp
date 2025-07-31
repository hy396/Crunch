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
	
}

void UInventoryItemWidget::EmptySlot()
{
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
}

void UInventoryItemWidget::RightButtonClicked()
{
}

void UInventoryItemWidget::LeftButtonClicked()
{
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
