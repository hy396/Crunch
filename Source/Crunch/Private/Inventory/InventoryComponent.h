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
	
	// 尝试购买商店物品
	void TryPurchase(const UPDA_ShopItem* ItemToPurchase);
	// 获取当前金币数量
	float GetGold() const;
	// 获取库存容量
	FORCEINLINE int32 GetCapacity() const { return Capacity; }
	// 处理物品槽位变更
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlotNumber);
	// 通过句柄获取库存物品
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;


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

	/** 向库存添加新物品 */
	void GrantItem(const UPDA_ShopItem* NewItem);
	/*********************************************************/
	/*                   Client                              */
	/*********************************************************/
private:
	/** 客户端：处理物品添加通知 */
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPDA_ShopItem* Item);
};
