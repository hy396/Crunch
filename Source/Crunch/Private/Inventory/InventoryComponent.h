// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PDA_ShopItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

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

	// 尝试购买商店物品
	void TryPurchase(const UPDA_ShopItem* ItemToPurchase);
	// 获取当前金币数量
	float GetGold() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	/*********************************************************/
	/*                   Server RPCs                         */
	/*********************************************************/

	/** 服务器端：处理购买请求 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPDA_ShopItem* ItemToPurchase);
};
