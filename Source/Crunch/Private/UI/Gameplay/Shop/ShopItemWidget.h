// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Inventory/PDA_ShopItem.h"
#include "UI/Common/ItemWidget.h"
#include "ShopItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class UShopItemWidget : public UItemWidget, 
						public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	
	//~ Begin IUserObjectListEntry 接口实现
	// 当列表项绑定数据对象时调用（通常为UPA_ShopItem实例）
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//~ End IUserObjectListEntry 接口实现

	// 获取当前绑定的商店物品数据
	FORCEINLINE const UPDA_ShopItem* GetShopItem() const { return ShopItem; }
private:
	
	// 当前绑定的商店物品数据资产
	UPROPERTY()
	TObjectPtr<const UPDA_ShopItem> ShopItem;
};
