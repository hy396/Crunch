// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "InventoryItem.h"
#include "PDA_ShopItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// 委托声明：当新物品添加到库存时广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /*NewItem*/);
// 委托声明：当物品从库存移除时广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle& /*ItemHandle*/);

// 委托声明：当物品堆叠数量变化时广播
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangeDelegate, const FInventoryItemHandle&, int32 /*NewCount*/);


/**
 * 库存管理组件，负责处理物品的添加、移除、使用和交易逻辑
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	// 物品添加事件委托
	FOnItemAddedDelegate OnItemAdded;
	// 物品移除事件委托
	FOnItemRemovedDelegate OnItemRemoved;
	// 物品堆叠数量变化事件委托
	FOnItemStackCountChangeDelegate OnItemStackCountChanged;
	
	// 尝试激活指定句柄对应的物品
	void TryActivateItem(const FInventoryItemHandle& ItemHandle);
	// 尝试购买商店物品
	void TryPurchase(const UPDA_ShopItem* ItemToPurchase);
	// 出售指定物品
	void SellItem(const FInventoryItemHandle& ItemHandle);
	// 获取当前金币数量
	float GetGold() const;
	// 获取库存容量
	FORCEINLINE int32 GetCapacity() const { return Capacity; }
	// TODO:25/07/31更改为服务器调用函数
	// 处理物品槽位变更
	// UFUNCTION(Server, Reliable, WithValidation)
	// void ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlotNumber);
	// TODO:25/08/01 似乎并不需要进行高昂的服务器操作
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlotNumber);
	// 通过句柄获取库存物品
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

	// 检查指定物品类型是否已达库存上限
	bool IsFullFor(const UPDA_ShopItem* Item) const;

	// 检查所有槽位是否已被占用
	bool IsAllSlotOccupied() const;
	
	// 获取指定物品可堆叠的库存实例
	UInventoryItem* GetAvailableStackForItem(const UPDA_ShopItem* Item) const;

	/**
	 * 查找合成指定物品所需的材料
	 * @param Item 目标物品
	 * @param OutIngredients 输出找到的材料
	 * @param IngredientToIgnore 需要忽略的材料列表
	 * @return 是否找到全部材料
	 */
	bool FindIngredientForItem(const UPDA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients, const TArray<const UPDA_ShopItem*>& IngredientToIgnore = TArray<const UPDA_ShopItem*>{});
	
	// 尝试获取与商店物品对应的库存物品
	UInventoryItem* TryGetItemForShopItem(const UPDA_ShopItem* Item) const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	/** 库存容量（槽位数） */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capacity = 6;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	// 存储物品
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	/*********************************************************/
	/*                   Server RPCs                         */
	/*********************************************************/

	/** 服务器端：处理购买请求 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPDA_ShopItem* ItemToPurchase);

	/** 服务器端：处理物品激活请求 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItem(FInventoryItemHandle ItemHandle);

	/** 服务器端：处理物品出售请求 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FInventoryItemHandle ItemHandle);
	
	/** 向库存添加新物品 */
	void GrantItem(const UPDA_ShopItem* NewItem);

	/** 消耗物品（减少堆叠或移除） */
	void ConsumeItem(UInventoryItem* Item);
	
	/** 从库存完全移除物品 */
	void RemoveItem(UInventoryItem* Item);

	/** 尝试物品合成 */
	bool TryItemCombination(const UPDA_ShopItem* NewItem);
	// TODO:我觉得这里需要添加全新的购买逻辑，合成这种方式实在是太唐了
	/*********************************************************/
	/*                   Client                              */
	/*********************************************************/
private:
	/** 客户端：处理物品添加通知 */
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPDA_ShopItem* Item);

	/** 客户端：处理物品移除通知 */
	UFUNCTION(Client, Reliable)
	void Client_ItemRemoved(FInventoryItemHandle ItemHandle);
	
	/** 客户端：处理物品堆叠数量变更通知 */
	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount);
};
