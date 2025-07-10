// 幻雨喜欢小猫咪


#include "UI/Gameplay/OverHeadStatsGauge.h"

#include "GenericTeamAgentInterface.h"
#include "GAS/Core/CAttributeSet.h"

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
	}
}

void UOverHeadStatsGauge::SetHealthBarColor(ETeamAttitude::Type TargetTeam)
{
	if (HealthBar)
	{
		switch (TargetTeam)
		{
			case ETeamAttitude::Friendly:
				// 队友就不用管了，本来就绿绿的（本来我不想管的，后来AI不稳定）
				HealthBar->SetBarColor(FriendlyColor);
				break;
			case ETeamAttitude::Hostile:
				HealthBar->SetBarColor(HostileColor);
				break;
			default:
				HealthBar->SetBarColor(FLinearColor::Yellow);
				break;
		}
	}
}
