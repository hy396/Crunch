// 幻雨喜欢小猫咪


#include "GAS/Core/CHeroAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void UCHeroAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, PrevLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, NextLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, UpgradePoint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MaxLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, Gold, COND_None, REPNOTIFY_Always);

	// DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, AttackPowerGrowthRate, COND_None, REPNOTIFY_Always)
	// DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MagicPowerGrowthRate, COND_None, REPNOTIFY_Always)
	// DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MaxHealthGrowthRate, COND_None, REPNOTIFY_Always);
	// DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MaxManaGrowthRate, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, ManaRegen, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MagicPenetration, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, ArmorPenetrationPercent, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MagicPenetrationPercent, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, LifeSteal, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, MagicLifeSteal, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, CooldownReduction, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, CriticalStrikeChance, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, CriticalStrikeDamage, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, Toughness, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, DamageAmplification, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCHeroAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always)
}

void UCHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, Intelligence, OldValue);
}

void UCHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, Strength, OldValue);
}

void UCHeroAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, Experience, OldValue);
}

void UCHeroAttributeSet::OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, PrevLevelExperience, OldValue);
}

void UCHeroAttributeSet::OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, NextLevelExperience, OldValue);
}

void UCHeroAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, Level, OldValue);
}

void UCHeroAttributeSet::OnRep_UpgradePoint(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, UpgradePoint, OldValue);
}

void UCHeroAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, MaxLevel, OldValue);
}

void UCHeroAttributeSet::OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, MaxLevelExperience, OldValue);
}

void UCHeroAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, Gold, OldValue);
}

void UCHeroAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, HealthRegen, OldValue);
}

void UCHeroAttributeSet::OnRep_ManaRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, ManaRegen, OldValue);
}

void UCHeroAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, ArmorPenetration, OldValue);
}

void UCHeroAttributeSet::OnRep_MagicPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, MagicPenetration, OldValue);
}

void UCHeroAttributeSet::OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, ArmorPenetrationPercent, OldValue);
}

void UCHeroAttributeSet::OnRep_MagicPenetrationPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, MagicPenetrationPercent, OldValue);
}

void UCHeroAttributeSet::OnRep_LifeSteal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, LifeSteal, OldValue);
}

void UCHeroAttributeSet::OnRep_MagicLifeSteal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, MagicLifeSteal, OldValue);
}

void UCHeroAttributeSet::OnRep_CooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, CooldownReduction, OldValue);
}

void UCHeroAttributeSet::OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, CriticalStrikeChance, OldValue);
}

void UCHeroAttributeSet::OnRep_CriticalStrikeDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, CriticalStrikeDamage, OldValue);
}

void UCHeroAttributeSet::OnRep_Toughness(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, Toughness, OldValue);
}

void UCHeroAttributeSet::OnRep_DamageAmplification(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, DamageAmplification, OldValue);
}

void UCHeroAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCHeroAttributeSet, DamageReduction, OldValue);
}
