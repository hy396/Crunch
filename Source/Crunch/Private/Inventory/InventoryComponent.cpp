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


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());

}

void UInventoryComponent::Server_Purchase_Implementation(const UPDA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;

	// 金币不够无法购买
	if (GetGold() < ItemToPurchase->GetPrice()) return;

	// 扣掉金币
	OwnerAbilitySystemComponent->ApplyModToAttribute(UCHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());

	// 修复日志输出：使用正确的字符串格式化方式
	const FString ItemName = ItemToPurchase->GetItemName().BuildSourceString();
	UE_LOG(LogTemp, Warning, TEXT("购买的物品: %s"), *ItemName);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPDA_ShopItem* ItemToPurchase)
{
	return true;
}

