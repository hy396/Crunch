// 幻雨喜欢小猫咪


#include "InventoryItemWidget.h"

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

void UInventoryItemWidget::UpdateCanCastDisplay(bool bCanCast)
{
}
