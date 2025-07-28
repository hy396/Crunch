// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Inventory/PDA_ShopItem.h"
#include "CAssetManager.generated.h"

/**
 * 自定义资产管理器，负责游戏核心资产的加载和管理
 * 处理角色定义、商店物品加载，并提供物品合成系统的数据支持
 */
UCLASS()
class UCAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	// 获取资产管理器单例
	static UCAssetManager& Get();

	/**
	 * 异步加载所有商店物品资产
	 * @param LoadFinishedCallback - 加载完成时执行的回调
	 */
	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback);

	/**
	 * 获取已加载的商店物品
	 * @param OutItems - 输出加载的商店物品数组
	 * @return 是否成功获取
	 */
	bool GetLoadedShopItems(TArray<const UPDA_ShopItem*>& OutItems) const;

private:
	// 商店物品加载完成后的处理
	void ShopItemLoadFinished(FStreamableDelegate Callback);
	
	// 构建物品关系映射（合成配方系统）
	void BuildItemMaps();
	
	/**
	 * 添加物品关系到合成映射
	 * @param Ingredient - 材料物品
	 * @param CombinationItem - 能合成的目标物品
	 */
	void AddToCombinationMap(const UPDA_ShopItem* Ingredient, const UPDA_ShopItem* CombinationItem);

	/** 合成配方映射：材料物品 -> 能合成的物品集合 */
	UPROPERTY()
	TMap<const UPDA_ShopItem*, FItemCollection> CombinationMap;

	/** 材料需求映射：目标物品 -> 所需材料集合 */
	UPROPERTY()
	TMap<const UPDA_ShopItem*, FItemCollection> IngredientMap;
};
