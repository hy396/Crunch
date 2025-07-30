// 幻雨喜欢小猫咪


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Core/CHeroAttributeSet.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::TryPurchase(const UPDA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent) return;

	// 在服务器中进行购买
	Server_Purchase(ItemToPurchase);
}

float UInventoryComponent::GetGold() const
{
	bool bFound = false;
	if (OwnerAbilitySystemComponent)
	{
		// 获取金币属性
		float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
		{
			return Gold;
		}
	}
	return 0.f;
}

void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlotNumber)
{
	// 通过句柄查找物品，并为其设置新插槽
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(NewSlotNumber);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
	// 通过句柄在Map中查找
	UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());

}

void UInventoryComponent::GrantItem(const UPDA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority()) return; // 确保服务器调用

	if (NewItem)
	{
		// 创建新物品
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		InventoryItem->InitItem(NewHandle, NewItem, OwnerAbilitySystemComponent);

		// 添加到库存中
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);
		UE_LOG(LogTemp, Warning, TEXT("服务器中添加的物品: %s, 唯一ID: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
		// FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();

		// 通知客户端
		Client_ItemAdded(NewHandle, NewItem);
	}
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle,
                                                          const UPDA_ShopItem* Item)
{
	if (GetOwner()->HasAuthority()) return;// 确保客户端调用

	if (Item)
	{
		// 创建本地物品副本
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		InventoryItem->InitItem(AssignedHandle, Item, OwnerAbilitySystemComponent);
		// InventoryItem->SetGarbageEliminationEnabled(GrantedAbilitySpecHandle)
		// 添加到本地库存
		InventoryMap.Add(AssignedHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);
		UE_LOG(LogTemp, Warning, TEXT("客户端中添加的物品: %s, 唯一ID: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
	}
}

void UInventoryComponent::Server_Purchase_Implementation(const UPDA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;

	// 金币不够无法购买
	if (GetGold() < ItemToPurchase->GetPrice()) return;
	// 背包不够也不能购买
	if (InventoryMap.Num() >= GetCapacity()) return;

	// 扣掉金币
	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	// 添加物品
	GrantItem(ItemToPurchase);
	// 修复日志输出：使用正确的字符串格式化方式
	// const FString ItemName = ItemToPurchase->GetItemName().BuildSourceString();
	// UE_LOG(LogTemp, Warning, TEXT("购买的物品: %s"), *ItemName);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPDA_ShopItem* ItemToPurchase)
{
	return true;
}

