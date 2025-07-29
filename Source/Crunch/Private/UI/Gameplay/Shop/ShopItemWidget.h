// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Inventory/PDA_ShopItem.h"
#include "UI/Common/ItemWidget.h"
#include "ShopItemWidget.generated.h"


class UShopItemWidget;

// 声明委托：当物品购买请求发出时触发（参数：商店物品数据资产）
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssused, const UPDA_ShopItem*);

// 声明委托：当商店物品被选中时触发（参数：商店物品控件实例）
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 * 
 */
UCLASS()
class UShopItemWidget : public UItemWidget, 
						public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	// 委托：物品购买请求发出时广播
	FOnItemPurchaseIssused OnItemPurchaseIssued;
	
	// 委托：物品被点击选择时广播
	FOnShopItemSelected OnShopItemClicked;
	
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

	//~ Begin UItemWidget 重写
	// 右键点击处理：触发购买委托
	virtual void RightButtonClicked() override;
	
	// 左键点击处理：触发选择委托
	virtual void LeftButtonClicked() override;
	//~ End UItemWidget 重写
};
