// 幻雨喜欢小猫咪


#include "UI/Gameplay/GameplayWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"


void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		// 绑定属性回调
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());

		// 绑定每秒回复的属性
		HealthBar->SetRegenValueTextToGameplayAttribute(OwnerAbilitySystemComponent, UCHeroAttributeSet::GetHealthRegenAttribute());
		ManaBar->SetRegenValueTextToGameplayAttribute(OwnerAbilitySystemComponent, UCHeroAttributeSet::GetManaRegenAttribute());
	}
}

void UGameplayWidget::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	AbilityListView->ConfigureAbilities(Abilities);
}
