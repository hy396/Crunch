// 幻雨喜欢小猫咪


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CGameplayAbilityTypes.h"

FInventoryItemHandle::FInventoryItemHandle()
	: HandleId{GetInvalidId()}
{
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	// 生成下一个ID的新句柄
	return FInventoryItemHandle(GenerateNextId());
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleId != GetInvalidId();
}

FInventoryItemHandle::FInventoryItemHandle(uint32 Id)
	: HandleId{Id}
{
}

uint32 FInventoryItemHandle::GenerateNextId()
{
	static uint32 StaticId = 1; // 从1开始计数
	return StaticId++;
}

uint32 FInventoryItemHandle::GetInvalidId()
{
	// 无效值为0
	return 0;
}

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
	// ID一样就一样
	return Lhs.GetHandleId() == Rhs.GetHandleId();
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleId();
}

UInventoryItem::UInventoryItem()
	: StackCount{1}, Slot(0)
// 默认堆叠数为1
{
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr;
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull())
	{
		// 堆叠数已满
		return false;
	}
	++StackCount;
	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	// 还有剩余的数量返回 true,没了返回 false
	return StackCount > 0;
}

bool UInventoryItem::SetStackCount(int32 NewStackCount)
{
	const UPDA_ShopItem* Item = GetShopItem();
	if (!Item) return false;

	// 传入的堆叠数量在合理区间
	if (NewStackCount > 0 && NewStackCount <= Item->GetMaxStackCount())
	{
		StackCount = NewStackCount;
		return true;
	}
	return false;
}

bool UInventoryItem::IsStackFull() const
{
	const UPDA_ShopItem* Item = GetShopItem();
	return Item && StackCount >= Item->GetMaxStackCount();
}

bool UInventoryItem::IsForItem(const UPDA_ShopItem* Item) const
{
	return Item && GetShopItem() == Item;
}

bool UInventoryItem::IsGrantingAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const
{
	// 是否为指定的技能
	return GetShopItem() && GetShopItem()->GetGrantedAbility() == AbilityClass;
}

bool UInventoryItem::IsGrantingAnyAbility() const
{
	// 是否有技能
	return GetShopItem() && GetShopItem()->GetGrantedAbility() != nullptr;
}

// TODO:在这里的变化可以尝试修改技能按键
void UInventoryItem::SetSlot(int32 NewSlot)
{
	Slot = NewSlot;
	if (OwnerAbilitySystemComponent->GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("服务器更新插槽"))
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("客户端更新插槽"))
		return;
	}
	// TODO:25/08/01 似乎不需要用这种操作，这种操作难以关联到太多的信息
	// // 定义槽位到输入ID的映射表（静态常量避免重复初始化）
	// static const TMap<int32, ECAbilityInputID> SlotInputMap = {
	// 	{0, ECAbilityInputID::AbilityOne},
	// 	{1, ECAbilityInputID::AbilityTwo},
	// 	{2, ECAbilityInputID::AbilityThree},
	// 	{3, ECAbilityInputID::AbilityFour},
	// 	{4, ECAbilityInputID::AbilityFive},
	// 	{5, ECAbilityInputID::AbilitySix}
	// };
	//
	// // 查找对应的输入ID（找不到时返回None）
	// const ECAbilityInputID SelectedInput = SlotInputMap.FindRef(Slot, ECAbilityInputID::None);
	// UE_LOG(LogTemp, Warning, TEXT("SelectedInput: %d"), SelectedInput)
 //    
	// // 提前退出条件：无效输入或无能力组件
	// if (SelectedInput == ECAbilityInputID::None || !OwnerAbilitySystemComponent) 
	// 	return;
	//
	// // 移除授予的能力重新绑定
	// if (GrantedAbilitySpecHandle.IsValid())
	// {
	// 	OwnerAbilitySystemComponent->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
	// 	if (GetShopItem()->GetGrantedAbility())
	// 	{
	// 		GrantedAbilitySpecHandle = OwnerAbilitySystemComponent->K2_GiveAbility(
	// 		GetShopItem()->GetGrantedAbility(),
	// 			1,
	// 			static_cast<int32>(SelectedInput));
	// 	}
	// }
	
	// 更新技能输入绑定
	// if (FGameplayAbilitySpec* Spec = OwnerAbilitySystemComponent->FindAbilitySpecFromHandle(GrantedAbilitySpecHandle))
	// {
	// 	Spec->InputID = static_cast<int32>(SelectedInput);
	// }
}

float UInventoryItem::GetAbilityCooldownTimeRemaining() const
{
	if (!IsGrantingAnyAbility() || !OwnerAbilitySystemComponent) return 0.f;
	
	return UCAbilitySystemStatics::GetCooldownRemainingFor(
		GetShopItem()->GetGrantedAbilityCDO(), 
		*OwnerAbilitySystemComponent
	);
}

float UInventoryItem::GetAbilityCooldownDuration() const
{
	if (!IsGrantingAnyAbility() || !OwnerAbilitySystemComponent) return 0.f;
	
	return UCAbilitySystemStatics::GetCooldownDurationFor(
		GetShopItem()->GetGrantedAbilityCDO(), 
		*OwnerAbilitySystemComponent, 
		1 // 默认能力等级
	);
}

// float UInventoryItem::GetAbilityManaCost() const
// {
// 	if (!IsGrantingAnyAbility() || !OwnerAbilitySystemComponent) return 0.f;
// 	
// 	return UCAbilitySystemStatics::GetManaCostFor(
// 		GetShopItem()->GetGrantedAbilityCDO(), 
// 		*OwnerAbilitySystemComponent, 
// 		1 // 默认能力等级
// 	);
// }

// bool UInventoryItem::CanCastAbility() const
// {
// 	if (!IsGrantingAnyAbility() || !OwnerAbilitySystemComponent) return false;
// 	
// 	// 优先通过能力规格检查
// 	if (FGameplayAbilitySpec* Spec = OwnerAbilitySystemComponent->FindAbilitySpecFromHandle(GrantedAbilitySpecHandle))
// 	{
// 		return UCAbilitySystemStatics::CheckAbilityCost(*Spec, *OwnerAbilitySystemComponent);
// 	}
// 	
// 	// 回退到通过CDO静态检查
// 	return UCAbilitySystemStatics::CheckAbilityCostStatic(
// 		GetShopItem()->GetGrantedAbilityCDO(), 
// 		*OwnerAbilitySystemComponent
// 	);
// }

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPDA_ShopItem* NewShopItem,
                              UAbilitySystemComponent* AbilitySystemComponent)
{
	Handle = NewHandle;				// 设置唯一句柄
	ShopItem = NewShopItem;			// 关联商店物品数据

	// 设置能力系统组件并绑定属性变化委托
	OwnerAbilitySystemComponent = AbilitySystemComponent;

	// TODO:移除法力变换
	// if (OwnerAbilitySystemComponent)
	// {
	// 	// 绑定法力变化
	// 	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	// 		UCAttributeSet::GetManaAttribute()
	// 	).AddUObject(this, &UInventoryItem::ManaUpdated);
	// }
	
	// 应用GAS修改
	ApplyGASModifications();
}

bool UInventoryItem::TryActivateGrantedAbility()
{
	if (!GrantedAbilitySpecHandle.IsValid()) return false;
	if (!OwnerAbilitySystemComponent) return false;

	// 激活技能，成功返回true
	return OwnerAbilitySystemComponent->TryActivateAbility(GrantedAbilitySpecHandle);
}

void UInventoryItem::ApplyConsumeEffect()
{
	if (!ShopItem) return;

	TArray<TSubclassOf<UGameplayEffect>> ConsumeEffects = GetShopItem()->GetConsumeEffects();
	// 应用消耗效果
	for (const TSubclassOf<UGameplayEffect>& ConsumeEffect : ConsumeEffects)
	{
		if (ConsumeEffect)
		{
			OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
				ConsumeEffect,
				1,
				OwnerAbilitySystemComponent->MakeEffectContext()
				);
		}
	}
}

void UInventoryItem::RemoveGASModifications()
{
	if (!OwnerAbilitySystemComponent) return;

	// TODO:等我移除法力的时候就得把这个一起移除了
	// 解除Mana属性变化委托
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCAttributeSet::GetManaAttribute()
	).RemoveAll(this);
	
	// 服务器端执行
	if (OwnerAbilitySystemComponent->GetOwner()->HasAuthority())
	{
		// 移除装备效果
		for (const FActiveGameplayEffectHandle& AppliedEquippedEffectHandle : AppliedEquippedEffectHandles)
		{
			if (AppliedEquippedEffectHandle.IsValid())
			{
				OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle);
			}
		}
		// 清空数组
		AppliedEquippedEffectHandles.Empty();

		// 移除技能
		if (GrantedAbilitySpecHandle.IsValid())
		{
			OwnerAbilitySystemComponent->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
		}
	}
}

void UInventoryItem::ApplyGASModifications()
{
	if (!GetShopItem() || !OwnerAbilitySystemComponent) return;
	// 仅服务器端执行
	if (!OwnerAbilitySystemComponent->GetOwner()->HasAuthority()) return;
	// 清除数组
	AppliedEquippedEffectHandles.Empty();
	// 应用装备效果
	TArray<TSubclassOf<UGameplayEffect>> EquipEffects = GetShopItem()->GetEquippedEffects();
	for (TSubclassOf<UGameplayEffect> EquipEffect : EquipEffects)
	{
		if (EquipEffect)
		{
			FActiveGameplayEffectHandle AppliedEquippedEffectHandle = OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
				EquipEffect,
				1,
				OwnerAbilitySystemComponent->MakeEffectContext()
				);
			AppliedEquippedEffectHandles.Add(AppliedEquippedEffectHandle);
		}
	}

	// 授予装备技能
	TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
	if (GrantedAbility)
	{
		GrantedAbilitySpecHandle = OwnerAbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(GrantedAbility)
			);
	}
}

// void UInventoryItem::ManaUpdated(const FOnAttributeChangeData& ChangeData)
// {
// 	// 技能可用委托更新
// 	OnAbilityCanCastUpdated.Broadcast(CanCastAbility());
// }
