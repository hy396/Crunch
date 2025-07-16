// 幻雨喜欢小猫咪


#include "UI/Gameplay/StatsGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	Icon->SetBrushFromTexture(IconTexture);
}

void UStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置为整数格式
	NumberFormattingOptions.MaximumFractionalDigits = 0;
	APawn* OwnerPlayerPawn = GetOwningPlayerPawn();
	if (!OwnerPlayerPawn) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPlayerPawn);
	if (OwnerASC)
	{
		bool bFound;
		float AttributeValue = OwnerASC->GetGameplayAttributeValue(Attribute, bFound);
		if (bFound)
		{
			SetValue(AttributeValue);
		}
		OwnerASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UStatsGauge::AttributeChanged);
	}
}

void UStatsGauge::SetValue(float NewVal)
{
	AttributeText->SetText(FText::AsNumber(NewVal, &NumberFormattingOptions));
}

void UStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data)
{
	SetValue(Data.NewValue);
}
