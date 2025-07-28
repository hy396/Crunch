// 幻雨喜欢小猫咪


#include "Framework/CAssetManager.h"

UCAssetManager& UCAssetManager::Get()
{
	// 尝试从引擎获取当前资产管理器实例
	UCAssetManager* Singleton = Cast<UCAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}

	// 如果获取失败，记录致命错误并创建新实例（安全后备）
	UE_LOG(LogLoad, Fatal, TEXT("资源管理器 必须是 CAssetManager 类型的实例"));
	return (*NewObject<UCAssetManager>());
}

// 加载所有商店物品类型的主资产，并在加载完成后触发回调
void UCAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	// 加载指定类型的主资产（商店物品）
	LoadPrimaryAssetsWithType(
		UPDA_ShopItem::GetShopItemAssetType(),		// 商店物品资产类型
		TArray<FName>(),							// 资产名称列表：空数组表示加载该类型所有资产
		FStreamableDelegate::CreateUObject(			// 创建绑定到当前对象的委托
			this,
			&UCAssetManager::ShopItemLoadFinished,  // 资产加载完成时触发的成员函数
			LoadFinishedCallback                    // 透传外部传入的回调委托
		)
	);
}

bool UCAssetManager::GetLoadedShopItems(TArray<const UPDA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	// 获取上商店物品主资产列表
	bool bLoaded = GetPrimaryAssetObjectList(
		UPDA_ShopItem::GetShopItemAssetType(),  // 商店物品资产类型
		LoadedObjects						// 存储加载的商店物品
		);

	if (bLoaded)
	{
		for (UObject* LoadedObject : LoadedObjects)
		{
			OutItems.Add(Cast<UPDA_ShopItem>(LoadedObject));
		}
	}

	return bLoaded;
}

// 商店物品加载完成后的处理
void UCAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	// 执行回调（通知外部加载完成）
	Callback.ExecuteIfBound();

	// 构建物品映射(合成系统)
	BuildItemMaps();
}

// 构建物品合成关系映射表
void UCAssetManager::BuildItemMaps()
{
	TArray<const UPDA_ShopItem*> LoadedItems;
	// 获取所有已加载的商店物品
	if (GetLoadedShopItems(LoadedItems))
	{
		// 遍历每一个物品
		for (const UPDA_ShopItem* Item : LoadedItems)
		{
			// 合成清单为空，则跳过
			if (Item->GetIngredients().Num() == 0)
			{
				continue;
			}

			TArray<const UPDA_ShopItem*> Items;
			// 处理每个合成材料
			for (const TSoftObjectPtr<UPDA_ShopItem>& Ingredient : Item->GetIngredients())
			{
				// 同步加载
				UPDA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				Items.Add(IngredientItem);
				
				// 添加到合成映射表（材料->可合成的物品）
				AddToCombinationMap(IngredientItem, Item);
			}

			// 添加到材料映射表（物品->所需材料）
			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

void UCAssetManager::AddToCombinationMap(const UPDA_ShopItem* Ingredient, const UPDA_ShopItem* CombinationItem)
{
	// 检查是否已存在该材料的记录
	FItemCollection* Combinations = CombinationMap.Find(Ingredient);
	if (Combinations)
	{
		// 确保不重复添加相同合成结果
		if (!Combinations->Contains(CombinationItem))
			Combinations->AddItem(CombinationItem);
	}
	else
	{
		// 创建新条目（材料->合成结果集合）
		CombinationMap.Add(Ingredient, FItemCollection{TArray<const UPDA_ShopItem*>{CombinationItem}});
	}
}
