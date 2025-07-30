// 幻雨喜欢小猫咪


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
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
	: StackCount{1} // 默认堆叠数为1
{
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr;
}

// TODO:在这里的变化可以尝试修改技能按键
void UInventoryItem::SetSlot(int32 NewSlot)
{
	Slot = NewSlot;
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPDA_ShopItem* NewShopItem,
                              UAbilitySystemComponent* AbilitySystemComponent)
{
	Handle = NewHandle;				// 设置唯一句柄
	ShopItem = NewShopItem;			// 关联商店物品数据

	// 设置能力系统组件并绑定属性变化委托
	OwnerAbilitySystemComponent = AbilitySystemComponent;

	// 应用GAS修改
	ApplyGASModifications();
}

void UInventoryItem::ApplyGASModifications()
{
	if (!GetShopItem() || !OwnerAbilitySystemComponent) return;
	if (!OwnerAbilitySystemComponent->GetOwner()->HasAuthority()) return;

	// 应用装备效果
	TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
	if (EquipEffect)
	{
		AppliedEquipedEffectHandle = OwnerAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(
			EquipEffect,
			1,
			OwnerAbilitySystemComponent->MakeEffectContext()
			);
	}

	// 授予装备技能
	TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
	if (GrantedAbility)
	{
		GrantedAbilitySpecHandle = OwnerAbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(GrantedAbility)
			);

		// TODO: 绑定输入按键，等插槽上线，使用枚举的方式来绑定插槽
		// GrantedAbiltiySpecHandle = OwnerAbilitySystemComponent->GiveAbility(
		// 	FGameplayAbilitySpec(GrantedAbility,
		// 		1,
		// 		static_cast<int32>(ECAbilityInputID::BasicAttack),
		// 		nullptr));
	}
}
