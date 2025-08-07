// 幻雨喜欢小猫咪


#include "ItemToolTip.h"

void UItemToolTip::SetItem(const UPDA_ShopItem* Item)
{
	// 设置名称
	ItemTitleText->SetText(Item->GetItemName());
	// 设置描述
	ItemDescriptionText->SetText(Item->GetItemDescription());
	// 设置价格
	ItemPriceText->SetText(FText::AsNumber(static_cast<int32>(Item->GetPrice())));
	// 设置图标
	IconImage->SetBrushFromTexture(Item->GetIcon());
}

void UItemToolTip::SetPrice(float newPrice)
{
	ItemPriceText->SetText(FText::AsNumber(static_cast<int32>(newPrice)));
}
