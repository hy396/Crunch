// 幻雨喜欢小猫咪


#include "ShopWidget.h"

#include "Framework/CAssetManager.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置可聚焦
	SetIsFocusable(true);
	// 加载物品
	LoadShopItems();

	// 绑定列表项生成事件
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetGenerated);

	// 获取玩家的库存组件并存储起来
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	}
}

void UShopWidget::LoadShopItems()
{
	// 调用资产管理器的异步加载方法
	// 加载完成后触发 ShopItemLoadFinished 回调
	UCAssetManager::Get().LoadShopItems(
		FStreamableDelegate::CreateUObject(this, &UShopWidget::ShopItemLoadFinished)
		);
}

void UShopWidget::ShopItemLoadFinished()
{
	// 获取所有已加载的商店物品
	TArray<const UPDA_ShopItem*> ShopItems;
	if (UCAssetManager::Get().GetLoadedShopItems(ShopItems))
	{
		// 添加商店物品
		for (const UPDA_ShopItem* ShopItem : ShopItems)
		{
			ShopItemList->AddItem(const_cast<UPDA_ShopItem*>(ShopItem));
		}
	}
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	// 转换为商店物品控件
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ItemWidget)
	{
		// 绑定购买事件到库存系统
		if (OwnerInventoryComponent)
		{
			ItemWidget->OnItemPurchaseIssued.AddUObject(
				OwnerInventoryComponent,
				&UInventoryComponent::TryPurchase);
		}
		// 添加到物品映射表
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);;
	}
}
