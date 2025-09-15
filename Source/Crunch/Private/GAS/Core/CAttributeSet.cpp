// 幻雨喜欢小猫咪


#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
// #include "Character/Interaction/CombatInterface.h"
#include "TGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CPlayerController.h"

void UCAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, BaseAttackDamage, COND_None, REPNOTIFY_Always);
	// DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, BaseMagicDamage, COND_None, REPNOTIFY_Always);
	// DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, BaseTrueDamage, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MagicDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, TrueDamage, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MagicPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MoveAcceleration, COND_None, REPNOTIFY_Always);
}

void UCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UCAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		SetCachedHealthPercent(GetHealth()/GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
		SetCachedManaPercent(GetMana()/GetMaxMana());
	}
	
	// 物理伤害
	if (Data.EvaluatedData.Attribute == GetAttackDamageAttribute())
	{
		float NewDamage = GetAttackDamage();
		SetAttackDamage(0.f);
		if (NewDamage > 0.f)
		{
			// UE_LOG(LogTemp, Warning, TEXT("物理: %f"), NewDamage)
			Damage(Props, TGameplayTags::DamageType_AttackDamage, NewDamage);
		}
	}
	
	// 魔法伤害
	if (Data.EvaluatedData.Attribute == GetMagicDamageAttribute())
	{
		float NewDamage = GetMagicDamage();
		SetMagicDamage(0.f);
		if (NewDamage > 0.f)
		{
			// UE_LOG(LogTemp, Warning, TEXT("魔法伤害: %f"), NewDamage)
			Damage(Props,TGameplayTags::DamageType_MagicDamage, NewDamage);
		}
	}
	
	// 真实伤害
	if (Data.EvaluatedData.Attribute == GetTrueDamageAttribute())
	{
		float NewDamage = GetTrueDamage();
		SetTrueDamage(0.f);
		if (NewDamage > 0.f)
		{
			// UE_LOG(LogTemp, Warning, TEXT("真实伤害: %f"), NewDamage)
			Damage(Props,TGameplayTags::DamageType_TrueDamage, NewDamage);
		}
	}
}

void UCAttributeSet::RescaleHealth()
{
	if (!GetOwningActor()->HasAuthority())
		return;

	if (GetCachedHealthPercent() != 0 && GetHealth() != 0)
	{
		SetHealth(GetMaxHealth() * GetCachedHealthPercent());
	}
}

void UCAttributeSet::RescaleMana()
{
	if (!GetOwningActor()->HasAuthority())
		return;

	if (GetCachedManaPercent() != 0 && GetMana() != 0)
	{
		SetMana(GetMaxMana() * GetCachedManaPercent());
	}
}

void UCAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, Health, OldHealth);
}

void UCAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MaxHealth, OldMaxHealth);
}

void UCAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, Mana, OldMana);
}

void UCAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MaxMana, OldMaxMana);
}

void UCAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, AttackDamage, OldValue);
}

void UCAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, Armor, OldValue);
}

void UCAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MoveSpeed, OldValue);
}

void UCAttributeSet::OnRep_MoveAcceleration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MoveAcceleration, OldValue);
}

void UCAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, AttackPower, OldValue);
}

void UCAttributeSet::OnRep_MagicPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MagicPower, OldValue);
}

void UCAttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MagicResistance, OldValue);
}

// void UCAttributeSet::OnRep_BaseAttackDamage(const FGameplayAttributeData& OldBaseAttackDamage)
// {
// 	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, BaseAttackDamage, OldBaseAttackDamage);
// }
//
// void UCAttributeSet::OnRep_BaseMagicDamage(const FGameplayAttributeData& OldBaseMagicDamage)
// {
// 	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, BaseMagicDamage, OldBaseMagicDamage);
// }
//
// void UCAttributeSet::OnRep_BaseTrueDamage(const FGameplayAttributeData& OldBaseTrueDamage)
// {
// 	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, BaseTrueDamage, OldBaseTrueDamage);
// }

void UCAttributeSet::OnRep_MagicDamage(const FGameplayAttributeData& OldMagicDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, MagicDamage, OldMagicDamage);
}

void UCAttributeSet::OnRep_TrueDamage(const FGameplayAttributeData& OldTrueDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, TrueDamage, OldTrueDamage);
}

void UCAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	//Source 效果的所有者   Target 效果应用的目标
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	//获取效果所有者的相关对象
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceAvatarActor != nullptr && Props.SourceController == nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->Controller;
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UCAttributeSet::Damage(const FEffectProperties& Props, FGameplayTag DamageType, const float Damage)
{
	bool bCriticalHit = false;
	float NewDamage = Damage;
	if (Props.SourceASC)
	{
		bool bFound = false;
		const float EffectiveCriticalHitChance = Props.SourceASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetCriticalStrikeChanceAttribute(), bFound);
		if (bFound)
		{
			bFound = false;
			bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
			if (bCriticalHit)
			{
				const float CriticalStrikeDamage = Props.SourceASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetCriticalStrikeDamageAttribute(), bFound);
				if (bFound)
				{
					NewDamage *= (1.f + CriticalStrikeDamage / 100.f);
					// UE_LOG(LogTemp, Warning, TEXT("暴击"))
				}
			}
		}
	}
			
	const float NewHealth = GetHealth() - NewDamage;
	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
	// UE_LOG(LogTemp, Log, TEXT("NewDamage: %f"), NewDamage)
			
	// 如果生命小于等于0触发死亡
	if (NewHealth <= 0.f)
	{
		// 触发死亡被动
		OnDeadAbility(Props);
	}
	// TODO:添加第四个参数，用来传递伤害的类型
	ShowFloatingText(Props,NewDamage, bCriticalHit, DamageType);
}

void UCAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsCriticalHit, FGameplayTag DamageType)
{
	// for (int32 i = 0; ;++i)
	// {
	// 	if (ACPlayerController* PC = Cast<ACPlayerController>(UGameplayStatics::GetPlayerController(TargetActor,i)))
	// 	{
	// 		PC->ShowDamageNumber(Damage, TargetActor, IsCriticalHit); //调用显示伤害数字
	// 	}else
	// 	{
	// 		break;
	// 	}
	// }

	// 从技能释放者身上获取PC并显示伤害数字
	if(ACPlayerController* PC = Cast<ACPlayerController>(Props.SourceCharacter->Controller))
	{
		PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsCriticalHit, DamageType); //调用显示伤害数字
	}
	// 从目标身上获取PC并显示伤害数字
	if(ACPlayerController* PC = Cast<ACPlayerController>(Props.TargetCharacter->Controller))
	{
		PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsCriticalHit, DamageType); //调用显示伤害数字
	}
}

void UCAttributeSet::OnDeadAbility(const FEffectProperties& Props)
{
	FGameplayEventData DeadAbilityEventData;
	if (Props.SourceAvatarActor)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Dead：%s"), *GetOwningActor()->GetName())
		DeadAbilityEventData.Target = Props.SourceAvatarActor;
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), 
		TGameplayTags::Stats_Dead, 
		DeadAbilityEventData);
}