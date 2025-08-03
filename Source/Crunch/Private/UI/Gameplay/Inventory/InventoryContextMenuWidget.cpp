// 幻雨喜欢小猫咪


#include "InventoryContextMenuWidget.h"

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetUseButtonClickedEvent() const
{
	return UseButton->OnClicked;
}

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetSellButtonClickedEvent() const
{
	return SellButton->OnClicked;
}
