// 幻雨喜欢小猫咪


#include "Inventory/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/CAssetManager.h"
#include "GAS/Core/CHeroAttributeSet.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
	// 查找物品
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem) return;
	
	// 服务器中激活物品
	Server_ActivateItem(ItemHandle);
}

void UInventoryComponent::TryPurchase(const UPDA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent) return;

	// 在服务器中进行购买
	Server_Purchase(ItemToPurchase);
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& ItemHandle)
{
	// 请求服务器出售物品
	Server_SellItem(ItemHandle);
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

// void UInventoryComponent::ItemSlotChanged_Implementation(const FInventoryItemHandle& Handle, int32 NewSlotNumber)
// {
// 	// 通过句柄查找物品，并为其设置新插槽
// 	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
// 	{
// 		FoundItem->SetSlot(NewSlotNumber);
// 	}
// }
//
// bool UInventoryComponent::ItemSlotChanged_Validate(const FInventoryItemHandle& Handle, int32 NewSlotNumber)
// {
// 	return true;
// }

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

// TODO:此函数稍加修改利用可以实现CalculateItemEffectivePrice的逻辑
bool UInventoryComponent::FindIngredientForItem(const UPDA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients,
	const TArray<const UPDA_ShopItem*>& IngredientToIgnore)
{
	// 获取物品合成所需材料
	const FItemCollection* Ingredients = UCAssetManager::Get().GetIngredientForItem(Item);
	if (!Ingredients) return false;

	bool bAllFound = true;
	// 遍历材料列表
	for (const UPDA_ShopItem* Ingredient : Ingredients->GetItems())
	{
		// 跳过忽略的素材
		if (IngredientToIgnore.Contains(Ingredient))
			continue;

		// 查找背包中是否有该物品
		UInventoryItem* FoundItem = TryGetItemForShopItem(Ingredient);
		if (!FoundItem)
		{
			// 缺一个就是找不到全部，返回false
			bAllFound = false;
			break;
		}
		// 背包中找到的物品
		OutIngredients.Add(FoundItem);
	}

	return bAllFound;
}

UInventoryItem* UInventoryComponent::TryGetItemForShopItem(const UPDA_ShopItem* Item) const
{
	if (!Item) return nullptr;

	// 遍历背包 寻找指定商品
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemHandlePair : InventoryMap)
	{
		// if (ItemHandlePair.Value && ItemHandlePair.Value->IsForItem(Item))
		if (ItemHandlePair.Value && ItemHandlePair.Value->GetShopItem() == Item)
		{
			return ItemHandlePair.Value;
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

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem) return;

	// 激活物品的技能
	InventoryItem->TryActivateGrantedAbility();
	const UPDA_ShopItem* Item = InventoryItem->GetShopItem();
	// 如果是消耗品则消耗
	if (Item->GetIsConsumable())
	{
		ConsumeItem(InventoryItem);
	}
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	// 只在服务器中执行
	if (!GetOwner()->HasAuthority()) return;
	if (!Item) return;
	// 应用消耗
	Item->ApplyConsumeEffect();
	// 减少物品堆叠一次, 如果物品堆叠数量为0则移除物品
	if (!Item->ReduceStackCount())
	{
		RemoveItem(Item);
	}else
	{
		// 广播物品堆叠变化
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		// 通知客户端物品堆叠变化
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority()) return;

	// 移除GAS的效果
	Item->RemoveGASModifications();
	OnItemRemoved.Broadcast(Item->GetHandle());
	// 从背包中移除物品
	InventoryMap.Remove(Item->GetHandle());
	Client_ItemRemoved(Item->GetHandle());
}

bool UInventoryComponent::TryItemCombination(const UPDA_ShopItem* NewItem)
{
	// 仅服务器调用
	if (!GetOwner()->HasAuthority()) return false;

	// 获取可合成的物品
	const FItemCollection* CombinationItems = UCAssetManager::Get().GetCombinationForItem(NewItem);

	if (!CombinationItems) return false;

	// 遍历所有的可合成物品
	for (const UPDA_ShopItem* CombinationItem : CombinationItems->GetItems())
	{
		TArray<UInventoryItem*> Ingredients;
		// 查找合成所需材料
		if (!FindIngredientForItem(CombinationItem, Ingredients, TArray<const UPDA_ShopItem*>{NewItem}))
			continue;

		// 移除材料
		for (UInventoryItem* Ingredient : Ingredients)
		{
			RemoveItem(Ingredient);
		}

		// 合成物品，递归购买，向下寻找有没有合成物，多叉树结构
		GrantItem(CombinationItem);
		return true;
	}

	return false;
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
	// 服务端出售物品
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem || !InventoryItem->IsValid()) return;
	if (!OwnerAbilitySystemComponent) return;

	// 获取售出价格，并给玩家添加金币
	float SellPrice = InventoryItem->GetShopItem()->GetSellPrice();

	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice * InventoryItem->GetStackCount());
	// 移除物品
	// RemoveItem(InventoryItem);
	// TODO:如果物品是堆叠的，则需要计算价格，205/08/04修改，不知道是否有bug
	// 物品可以堆叠
	if (InventoryItem->GetShopItem()->GetIsStackable())
	{
		// 将物品乘堆叠数量,移除物品
		// SellPrice *= InventoryItem->GetStackCount();
		// 广播物品堆叠变化
		if (!InventoryItem->ReduceStackCount())
		{
			RemoveItem(InventoryItem);
		}else
		{
			// 广播物品堆叠变化
			OnItemStackCountChanged.Broadcast(InventoryItem->GetHandle(), InventoryItem->GetStackCount());
			// 通知客户端物品堆叠变化
			Client_ItemStackCountChanged(InventoryItem->GetHandle(), InventoryItem->GetStackCount());
		}
	}else
	{
		// 移除物品(非堆叠物品直接移除)
		RemoveItem(InventoryItem);
	}
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
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
			// TODO: 物品合成也是一个逆天的存在，应该取消掉
			// 尝试物品合成
			if (TryItemCombination(NewItem))
			{
				return;
			}
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

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle ItemHandle)
{
	// 客户端移除物品
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = GetInventoryItemByHandle(ItemHandle);
	if (!InventoryItem) return;
	// 移除GAS效果
	InventoryItem->RemoveGASModifications();
	OnItemRemoved.Broadcast(ItemHandle);
	// 从背包中移除物品
	InventoryMap.Remove(ItemHandle);
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

	// TODO:购买逻辑的金币`(ItemToPurchase->GetPrice())`此值或许需要进行传递的方式把价值传过去,或许需要修改
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

