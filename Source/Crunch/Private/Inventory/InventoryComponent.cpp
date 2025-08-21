// 幻雨喜欢小猫咪


#include "InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "InventoryItem.h"
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

void UInventoryComponent::TryActivateItemInSlot(int32 SlotNumber)
{
	// 遍历背包查找指定槽位的物品并激活
	for (TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (ItemPair.Value->GetItemSlot() == SlotNumber)
		{
			Server_ActivateItem(ItemPair.Key);
			return;
		}
	}
}

TArray<FInventoryItemHandle> UInventoryComponent::TryGetItemForShopItemHandles(const UPDA_ShopItem* Item) const
{
	if (!Item) return TArray<FInventoryItemHandle>();

	// 寻找背包中所有跟该物品有关的句柄,将句柄放入到数组中
	TArray<FInventoryItemHandle> FoundHandles;
	for (const TPair<FInventoryItemHandle, UInventoryItem*>& ItemHandlePair : InventoryMap)
	{
		// if (ItemHandlePair.Value && ItemHandlePair.Value->IsForItem(Item))
		if (ItemHandlePair.Value && ItemHandlePair.Value->GetShopItem() == Item)
		{
			FoundHandles.Add(ItemHandlePair.Key);
		}
	}
	return FoundHandles;
}

float UInventoryComponent::GetPurchasePrice(const UPDA_ShopItem* Item,
	TArray<FInventoryItemHandle>& OutItemHandles) const
{
	if (!Item) return 0.f;

	// 初始化价格
	float PurchasePrice = Item->GetPrice();
	// 寻找合成材料
	FindCombinationForItem(Item, OutItemHandles);
	for (const FInventoryItemHandle& Handle : OutItemHandles)
	{
		// 根据句柄寻找物品，如果找到了，则开始修改价格
		if (UInventoryItem* FoundItem =GetInventoryItemByHandle(Handle))
		{
			// 获取物品价格
			float IngredientPrice = FoundItem->GetShopItem()->GetPrice();
			// 修改价格
			PurchasePrice -= IngredientPrice;
		}
	}
	return PurchasePrice;
}

void UInventoryComponent::FindCombinationForItem(const UPDA_ShopItem* Item,
	TArray<FInventoryItemHandle>& OutItemHandles) const
{
	// 获取物品合成所需材料
	const FItemCollection* Ingredients = UCAssetManager::Get().GetIngredientForItem(Item);
	if (!Ingredients) return ;

	// 遍历材料列表
	for (const UPDA_ShopItem* Ingredient : Ingredients->GetItems())
	{
		UE_LOG(LogTemp, Log, TEXT("子节点物品: %s"), *Ingredient->GetItemName().ToString())
		// 获取背包中所有该物品的句柄
		TArray<FInventoryItemHandle> Temp = TryGetItemForShopItemHandles(Ingredient);
		// 是否找到物品
		bool bFound = false;
		for (const FInventoryItemHandle& Handle : Temp)
		{
			// 忽略重复句柄
			if (OutItemHandles.Contains(Handle))
			{
				continue;
			}
			// 找到后,添加句柄,并退出循环（避免添加更多的句柄进来造成无用的浪费）（其实是造成错误）
			bFound = true;
			OutItemHandles.AddUnique(Handle);
			break;
		}
		// 如果没有找到物品则继续向下寻找
		if (!bFound)
		{
			FindCombinationForItem(Ingredient, OutItemHandles);
		}
	}
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());

	// 绑定能力释放回调
	if (OwnerAbilitySystemComponent)
		OwnerAbilitySystemComponent->AbilityCommittedCallbacks.AddUObject(this, &UInventoryComponent::AbilityCommitted);
}

void UInventoryComponent::AbilityCommitted(class UGameplayAbility* CommittedAbility)
{
	if (!CommittedAbility) return;

	float CooldownTimeRemaining = 0.f;
	float CooldownDuration = 0.f;

	// 获取技能冷却时间
	CommittedAbility->GetCooldownTimeRemainingAndDuration(
		CommittedAbility->GetCurrentAbilitySpecHandle(),
		CommittedAbility->GetCurrentActorInfo(),
		CooldownTimeRemaining,
		CooldownDuration
	);

	// 遍历背包，找到授予该技能的物品，广播冷却信息
	for (TPair<FInventoryItemHandle, UInventoryItem*>& ItemPair : InventoryMap)
	{
		if (!ItemPair.Value)
			continue;

		if (ItemPair.Value->IsGrantingAbility(CommittedAbility->GetClass()))
		{
			OnItemAbilityCommitted.Broadcast(ItemPair.Key, CooldownDuration, CooldownTimeRemaining);
		}
	}
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

void UInventoryComponent::GrantItem(const UPDA_ShopItem* NewItem, float PurchasePrice,
	TArray<FInventoryItemHandle> RemoveHandles)
{
	// 仅限服务端
	if (!GetOwner()->HasAuthority()) return;
	// 尝试堆叠
	if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
	{
		StackItem->AddStackCount();
		// 通知堆叠数量变化
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
		Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
	}else
	{
		// 去掉背包中需要移除的物品
		for (const FInventoryItemHandle& Handle : RemoveHandles)
		{
			RemoveItem(GetInventoryItemByHandle(Handle));
		}
		
		// 创建新物品对象
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		// 初始化物品
		InventoryItem->InitItem(NewHandle, NewItem, OwnerAbilitySystemComponent);
		// 添加到背包
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);

		UE_LOG(LogTemp, Warning, TEXT("服务器中添加的物品: %s, 唯一ID: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
		FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();

		// 通知客户端
		Client_ItemAdded(NewHandle, NewItem, GrantedAbilitySpecHandle);
	}
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
			FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();
			// 通知客户端
			Client_ItemAdded(NewHandle, NewItem, GrantedAbilitySpecHandle);
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
                                                          const UPDA_ShopItem* Item, FGameplayAbilitySpecHandle GrantedAbilitySpecHandle)
{
	if (GetOwner()->HasAuthority()) return;// 确保客户端调用

	if (Item)
	{
		// 创建本地物品副本
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		// 先传给他在初始化，反正这个句柄的修改在服务器，这里是客户端
		InventoryItem->SetGrantedAbilitySpecHandle(GrantedAbilitySpecHandle);
		UE_LOG(LogTemp, Warning, TEXT("设置了GrantedAbilitySpecHandle"))
		InventoryItem->InitItem(AssignedHandle, Item, OwnerAbilitySystemComponent);
		// InventoryItem->SetGrantedAbilitySpecHandle(GrantedAbilitySpecHandle);
		// 添加到本地库存
		InventoryMap.Add(AssignedHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);
		UE_LOG(LogTemp, Warning, TEXT("客户端中添加的物品: %s, 唯一ID: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
	}
}

void UInventoryComponent::Server_Purchase_Implementation(const UPDA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;

	// 需要移除的物品
	TArray<FInventoryItemHandle> Ingredients;
	// 购买物品的金额
	float PurchasePrice = GetPurchasePrice(ItemToPurchase,Ingredients);
	
	// 金币不够无法购买(理论上金币是够的)(新添加了双击购买，理论失败，退回了右键购买，理论失败)
	if (GetGold() < PurchasePrice) return;
	
	// TODO:25/08/04修改的堆叠以及自动合成机制，不知是否有BUG先todo一手
	// 物品可以堆叠，并且有位置放
	if (GetAvailableStackForItem(ItemToPurchase))
	{
		// 扣掉金币
		OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -PurchasePrice);
		GrantItem(ItemToPurchase, PurchasePrice,Ingredients);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("移除的数量: %d，背包格子：%d"), Ingredients.Num(),InventoryMap.Num())
	// 不可堆叠物,计算参与合成后的位置
	// 计算格子
	if (InventoryMap.Num() - Ingredients.Num() + 1 > GetCapacity()) return;
	// 格子足够，直接购买
	// 扣掉金币
	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -PurchasePrice);
	GrantItem(ItemToPurchase, PurchasePrice,Ingredients);
	

	

	
	// // 检查金币是否足够
	// if (GetGold() < ItemToPurchase->GetPrice())
	// 	return;
	//
	// // 背包未满则直接购买
	// if (!IsFullFor(ItemToPurchase))
	// {
	// 	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	// 	GrantItem(ItemToPurchase);
	// 	return;
	// }
	//
	// // 背包满时尝试合成
	// if (TryItemCombination(ItemToPurchase))
	// {
	// 	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	// }
}

bool UInventoryComponent::Server_Purchase_Validate(const UPDA_ShopItem* ItemToPurchase)
{
	return true;
}

