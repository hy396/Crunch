// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ShopItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TileView.h"
#include "Inventory/PDA_ShopItem.h"
#include "ShopWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	// 商店物品列表
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> ShopItemList;

	// 加载商店物品
	void LoadShopItems();

	// 商店物品加载完成
	void ShopItemLoadFinished();

	// 商店物品生成
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);
	
	// 商店物品到控件的映射表
	// 用途：快速查找物品对应的控件实例
	UPROPERTY()
	TMap<const UPDA_ShopItem*, const UShopItemWidget*> ItemsMap;
};
