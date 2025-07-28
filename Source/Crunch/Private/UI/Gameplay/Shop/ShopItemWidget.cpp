// 幻雨喜欢小猫咪


#include "ShopItemWidget.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ShopItem = Cast<UPDA_ShopItem>(ListItemObject);
	if (!ShopItem) return;
	
	SetIcon(ShopItem->GetIcon());
}
