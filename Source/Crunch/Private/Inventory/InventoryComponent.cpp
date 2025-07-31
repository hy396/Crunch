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

// void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlotNumber)
// {
// 	// 通过句柄查找物品，并为其设置新插槽
// 	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
// 	{
// 		FoundItem->SetSlot(NewSlotNumber);
// 	}
// }

void UInventoryComponent::ItemSlotChanged_Implementation(const FInventoryItemHandle& Handle, int32 NewSlotNumber)
{
	// 通过句柄查找物品，并为其设置新插槽
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(NewSlotNumber);
	}
}

bool UInventoryComponent::ItemSlotChanged_Validate(const FInventoryItemHandle& Handle, int32 NewSlotNumber)
{
	return true;
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

bool UInventoryComponent::IsFullFor(const UPDA_ShopItem* Item) const
{
	if (!Item) return false;

	// 所有格子占满了，判断是否可以堆叠
	if (IsAllSlotOccupied())
	{
		return GetAvailableStackForItem(Item) == nullptr;
	}
	return false;
}

bool UInventoryComponent::IsAllSlotOccupied() const
{
	// 背包格子是否满了
	return InventoryMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UPDA_ShopItem* Item) const
{
	// 物品不可堆叠直接返回
	if (!Item->GetIsStackable())
		return nullptr;

	// 遍历背包查找可堆叠的物品
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (ItemPair.Value && ItemPair.Value->IsForItem(Item) && !ItemPair.Value->IsStackFull())
		{
			return ItemPair.Value;
		}
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
		// 判断物品是否可堆叠，可堆叠就遍历背包寻找可堆叠的物品
		if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
		{
			// 找到可以堆叠的物品，堆叠数量+1
			StackItem->AddStackCount();
			// 广播通知堆叠的变化          
			OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
			// 通知客户端堆叠变化
			Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
		}else
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
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
	if (GetOwner()->HasAuthority()) return; // 客户端执行
	
	// 根据句柄获取物品指针
	UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle);
	if (FoundItem)
	{
		// 设置物品数量
		FoundItem->SetStackCount(NewCount);
		// 广播执行数量变化
		OnItemStackCountChanged.Broadcast(Handle, NewCount);
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
	if (IsFullFor(ItemToPurchase)) return;

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

