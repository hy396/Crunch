// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "PDA_ShopItem.generated.h"

class UPDA_ShopItem;
/**
 * 表示商店物品集合的结构体，用于管理一组UPDA_ShopItem引用
 */
USTRUCT(BlueprintType)
struct FItemCollection
{
	GENERATED_BODY()
public:
	FItemCollection();

	// 使用现有物品列表初始化集合
	FItemCollection(const TArray<const UPDA_ShopItem*>& InItems);

	/**
	 * 向集合中添加新物品
	 * @param NewItem  要添加的物品
	 * @param bAddUnique  是否确保唯一性（默认false）
	 */
	void AddItem(const UPDA_ShopItem* NewItem, bool bAddUnique = false);

	/**
	 * 检查集合中是否包含指定物品
	 * @param Item 要检擦的物品
	 * @return 存在返回true，否则false
	 */
	bool Contains(const UPDA_ShopItem* Item) const;

	/**
	 * 获取集合中所有物品的引用
	 * @return 所有物品的引用 
	 */
	const TArray<const UPDA_ShopItem*>& GetItems() const;

private:
	TArray<TObjectPtr<const UPDA_ShopItem>> Items;
};

/**
 * 商店物品基础数据资产类，定义可在商店中交易的物品属性
 */
UCLASS()
class CRUNCH_API UPDA_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 获取物品ID
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// 获取商店物品的资产类型标识符
	static FPrimaryAssetType GetShopItemAssetType();

	// 获取物品图标
	UTexture2D* GetIcon() const;

	// 获取物品的名称
	FText GetItemName() const { return ItemName; }

	// 获取物品的描述
	FText GetItemDescription() const { return ItemDescription; }

	// 获取物品的购买价格
	float GetPrice() const { return Price; }

	// 获取物品的出售价格（设为购入价格的一半）
	float GetSellPrice() const { return Price / 2.0f; }

	// TODO:可以切换为数组,多个效果，然后有点效果采用堆叠的形式，限制堆叠大小为1
	// 获取装备时触发的GE
	TArray<TSubclassOf<UGameplayEffect>> GetEquippedEffects() const { return EquippedEffects; }
	
	// 获取使用时触发的GE
	TArray<TSubclassOf<UGameplayEffect>> GetConsumeEffects() const { return ConsumeEffects; }
	
	// 获取物品授予的GA
	TSubclassOf<UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }

	// 获取物品授予的GA的默认对象
	UGameplayAbility* GetGrantedAbilityCDO() const;

	// 检查物品是否可堆叠
	bool GetIsStackable() const { return bIsStackable; }

	// 检查物品是否可消耗
	bool GetIsConsumable() const { return bIsConsumable; }

	// 获取最大堆叠数量
	int32 GetMaxStackCount() const { return MaxStackCount; }

	// 获取合成所需的材料物品列表
	const TArray<TSoftObjectPtr<UPDA_ShopItem>>& GetIngredients() const { return IngredientItems; }

private:
	/** 物品图标资源引用 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "图标"))
	TSoftObjectPtr<UTexture2D> Icon;

	/** 物品基础购买价格 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "价格"))
	float Price;

	/** 物品显示名称 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "物品名称"))
	FText ItemName;

	/** 物品详细描述 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "物品描述", MultiLine = true))
	FText ItemDescription;

	/** 标识物品是否为消耗品 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "是否为消耗品"))
	bool bIsConsumable;

	// TODO:可以切换为数组,多个效果，然后有点效果采用堆叠的形式，限制堆叠大小为1
	/** 装备时应用的GameplayEffect */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "装备效果"))
	TArray<TSubclassOf<UGameplayEffect>> EquippedEffects;

	/** 使用时应用的GameplayEffect */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "使用效果"))
	TArray<TSubclassOf<UGameplayEffect>> ConsumeEffects;
	
	/** 物品授予的GameplayAbility */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "授予能力"))
	TSubclassOf<UGameplayAbility> GrantedAbility;
	// TODO:要不要加一个数组呢？用来存储被动技能

	/** 标识物品是否可堆叠 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "是否可堆叠"))
	bool bIsStackable = false;

	/** 最大堆叠数量（仅在可堆叠时有效） */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "最大堆叠数量"))
	int MaxStackCount = 5;

	/** 合成/制作所需的材料物品列表 */
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (DisplayName = "所需材料"))
	TArray<TSoftObjectPtr<UPDA_ShopItem>> IngredientItems;
};
