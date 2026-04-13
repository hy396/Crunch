// 幻雨喜欢小猫咪


#include "GAS/Core/GASDebugHelper.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AttributeSet.h"

DEFINE_LOG_CATEGORY(LogCrunchGAS);

void UGASDebugHelper::PrintASCState(UAbilitySystemComponent* ASC, bool bVerbose)
{
	if (!ASC)
	{
		UE_LOG(LogCrunchGAS, Warning, TEXT("[GAS Debug] ASC is null"));
		return;
	}

	AActor* Owner = ASC->GetOwnerActor();
	AActor* Avatar = ASC->GetAvatarActor();

	UE_LOG(LogCrunchGAS, Log, TEXT(""));
	UE_LOG(LogCrunchGAS, Log, TEXT("========== GAS STATE DUMP =========="));
	UE_LOG(LogCrunchGAS, Log, TEXT("  Owner:  %s"), Owner ? *Owner->GetName() : TEXT("null"));
	UE_LOG(LogCrunchGAS, Log, TEXT("  Avatar: %s"), Avatar ? *Avatar->GetName() : TEXT("null"));
	UE_LOG(LogCrunchGAS, Log, TEXT("  Authority: %s"),
		Owner && Owner->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));
	UE_LOG(LogCrunchGAS, Log, TEXT("  ReplicationMode: %d"), (int32)ASC->ReplicationMode);
	UE_LOG(LogCrunchGAS, Log, TEXT("===================================="));

	if (bVerbose)
	{
		PrintGrantedAbilities(ASC);
		PrintActiveEffects(ASC);
		PrintAttributes(ASC);
		PrintNetworkState(ASC);
	}
}

void UGASDebugHelper::PrintActiveEffects(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	UE_LOG(LogCrunchGAS, Log, TEXT(""));
	UE_LOG(LogCrunchGAS, Log, TEXT("-- Active GameplayEffects --"));

	const FActiveGameplayEffectsContainer& ActiveEffects = ASC->GetActiveGameplayEffects();
	int32 Count = 0;

	for (const FActiveGameplayEffect& ActiveGE : &ActiveEffects)
	{
		const FGameplayEffectSpec& Spec = ActiveGE.Spec;
		FString EffectName = Spec.Def ? Spec.Def->GetName() : TEXT("Unknown");

		float Duration = ActiveGE.GetDuration();
		float TimeRemaining = ActiveGE.GetTimeRemaining(ASC->GetWorld()->GetTimeSeconds());
		int32 StackCount = ActiveGE.Spec.GetStackCount();

		UE_LOG(LogCrunchGAS, Log, TEXT("  [%d] %s (Duration:%.1f Remaining:%.1f Stacks:%d Level:%.0f)"),
			Count, *EffectName, Duration, TimeRemaining, StackCount, Spec.GetLevel());
		Count++;
	}

	if (Count == 0)
	{
		UE_LOG(LogCrunchGAS, Log, TEXT("  (no active effects)"));
	}
	UE_LOG(LogCrunchGAS, Log, TEXT("  Total: %d"), Count);
}

void UGASDebugHelper::PrintGrantedAbilities(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	UE_LOG(LogCrunchGAS, Log, TEXT(""));
	UE_LOG(LogCrunchGAS, Log, TEXT("-- Granted Abilities --"));

	const TArray<FGameplayAbilitySpec>& Specs = ASC->GetActivatableAbilities();

	for (const FGameplayAbilitySpec& Spec : Specs)
	{
		FString AbilityName = Spec.Ability ? Spec.Ability->GetName() : TEXT("Unknown");
		bool bActive = Spec.IsActive();

		float CooldownRemaining = 0.f;
		float CooldownDuration = 0.f;
		if (Spec.Ability)
		{
			const FGameplayTagContainer* CooldownTags = Spec.Ability->GetCooldownTags();
			if (CooldownTags && CooldownTags->Num() > 0)
			{
				// 通过查询冷却GE的剩余时间来获取CD信息
				FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(*CooldownTags);
				TArray<float> Durations = ASC->GetActiveEffectsTimeRemaining(Query);
				if (Durations.Num() > 0)
				{
					CooldownRemaining = Durations[0];
				}
				TArray<float> FullDurations = ASC->GetActiveEffectsDuration(Query);
				if (FullDurations.Num() > 0)
				{
					CooldownDuration = FullDurations[0];
				}
			}
		}

		UE_LOG(LogCrunchGAS, Log, TEXT("  %s [%s] Level:%d CD:%.1f/%.1f InputID:%d"),
			*AbilityName,
			bActive ? TEXT("ACTIVE") : TEXT("ready"),
			Spec.Level,
			CooldownRemaining, CooldownDuration,
			Spec.InputID);
	}

	UE_LOG(LogCrunchGAS, Log, TEXT("  Total: %d"), Specs.Num());
}

void UGASDebugHelper::PrintAttributes(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	UE_LOG(LogCrunchGAS, Log, TEXT(""));
	UE_LOG(LogCrunchGAS, Log, TEXT("-- Attributes --"));

	// 遍历ASC的SpawnedAttributes获取所有AttributeSet
	const TArray<UAttributeSet*>& AttributeSets = ASC->GetSpawnedAttributes();

	for (const UAttributeSet* Set : AttributeSets)
	{
		UE_LOG(LogCrunchGAS, Log, TEXT("  [%s]"), *Set->GetClass()->GetName());

		for (TFieldIterator<FProperty> It(Set->GetClass()); It; ++It)
		{
			FProperty* Property = *It;
			if (FGameplayAttribute::IsGameplayAttributeDataProperty(Property))
			{
				FGameplayAttribute Attr(Property);
				float BaseValue = ASC->GetNumericAttributeBase(Attr);
				float CurrentValue = ASC->GetNumericAttribute(Attr);

				UE_LOG(LogCrunchGAS, Log, TEXT("    %s: Base=%.2f Current=%.2f%s"),
					*Property->GetName(), BaseValue, CurrentValue,
					FMath::IsNearlyEqual(BaseValue, CurrentValue) ? TEXT("") : TEXT(" (MODIFIED)"));
			}
		}
	}
}

void UGASDebugHelper::PrintNetworkState(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	UE_LOG(LogCrunchGAS, Log, TEXT(""));
	UE_LOG(LogCrunchGAS, Log, TEXT("-- Network State --"));

	const TCHAR* ModeStr = TEXT("Unknown");
	switch (ASC->ReplicationMode)
	{
	case EGameplayEffectReplicationMode::Full:    ModeStr = TEXT("Full"); break;
	case EGameplayEffectReplicationMode::Mixed:   ModeStr = TEXT("Mixed"); break;
	case EGameplayEffectReplicationMode::Minimal: ModeStr = TEXT("Minimal"); break;
	}
	UE_LOG(LogCrunchGAS, Log, TEXT("  ReplicationMode: %s"), ModeStr);

	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);
	UE_LOG(LogCrunchGAS, Log, TEXT("  ActiveTags (%d): %s"), OwnedTags.Num(), *OwnedTags.ToString());
}

void UGASDebugHelper::RegisterPredictionFailureMonitor(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	ASC->AbilityFailedCallbacks.AddLambda(
		[](const UGameplayAbility* Ability, const FGameplayTagContainer& FailureTags)
		{
			UE_LOG(LogCrunchGAS, Warning,
				TEXT("[GAS] Ability FAILED: %s | Reason: %s"),
				Ability ? *Ability->GetName() : TEXT("null"),
				*FailureTags.ToString());
		}
	);

	ASC->AbilityActivatedCallbacks.AddLambda(
		[](UGameplayAbility* Ability)
		{
			UE_LOG(LogCrunchGAS, Verbose,
				TEXT("[GAS] Ability ACTIVATED: %s (Predicting: %s)"),
				Ability ? *Ability->GetName() : TEXT("null"),
				Ability && Ability->GetCurrentActivationInfo().ActivationMode ==
					EGameplayAbilityActivationMode::Predicting ? TEXT("Yes") : TEXT("No"));
		}
	);

	ASC->AbilityEndedCallbacks.AddLambda(
		[](UGameplayAbility* Ability)
		{
			UE_LOG(LogCrunchGAS, Verbose,
				TEXT("[GAS] Ability ENDED: %s"),
				Ability ? *Ability->GetName() : TEXT("null"));
		}
	);

	UE_LOG(LogCrunchGAS, Log, TEXT("[GAS Debug] Monitor registered for %s"),
		ASC->GetOwnerActor() ? *ASC->GetOwnerActor()->GetName() : TEXT("Unknown"));
}
