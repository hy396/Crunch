// 幻雨喜欢小猫咪


#include "ShopItemWidget.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ShopItem = Cast<UPDA_ShopItem>(ListItemObject);
	if (!ShopItem) return;
	
	SetIcon(ShopItem->GetIcon());

	SetToolTipWidget(ShopItem);
}

void UShopItemWidget::RightButtonClicked()
{
	// 购买
	OnItemPurchaseIssued.Broadcast(GetShopItem());
}

void UShopItemWidget::LeftButtonClicked()
{
	// 选中
	OnShopItemClicked.Broadcast(this);
}
