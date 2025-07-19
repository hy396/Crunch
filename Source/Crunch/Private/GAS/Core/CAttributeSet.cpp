// 幻雨喜欢小猫咪


#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
// #include "Character/Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CPlayerController.h"

void UCAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCAttributeSet, AttackPowerCoefficient, COND_None, REPNOTIFY_Always);
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
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		//SetCachedHealthPercent(GetHealth()/GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
		//SetCachedManaPercent(GetMana()/GetMaxMana());
	}

	// 伤害
	
	if (Data.EvaluatedData.Attribute == GetAttackDamageAttribute())
	{
		FEffectProperties Props;
		SetEffectProperties(Data, Props);
		float NewDamage = GetAttackDamage();
		SetAttackDamage(0.f);
		bool bCriticalHit = false;
		if (NewDamage > 0.f)
		{
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
			UE_LOG(LogTemp, Warning, TEXT("NewDamage: %f"), NewDamage)
			
			// 显示伤害数字（Aura的方法）失败
			// ShowFloatingText(Props,NewDamage, bCriticalHit);
			// Client_ShowFloatingText_Implementation(Props,NewDamage, bCriticalHit);
			
			// GC 的方法，失败
			// if (Props.SourceASC)
			// {
			// 	FGameplayCueParameters BlastingGameplayCueParameters;
			// 	if (AActor* Target = Data.Target.AbilityActorInfo->AvatarActor.Get())
			// 	{
			// 		// 设置特效的位置
			// 		BlastingGameplayCueParameters.Location = Target->GetActorLocation();
			// 		// 随便找一个变量设置暴击
			// 		BlastingGameplayCueParameters.NormalizedMagnitude = bCriticalHit ? 1.f : 0.f;
			// 	}
			// 	// if (AActor* Source = Props.SourceASC->AbilityActorInfo->AvatarActor.Get())
			// 	// {
			// 	// 	BlastingGameplayCueParameters.SourceObject = Source;
			// 	// }
			// 	// 随便找一个变量存伤害值
			// 	BlastingGameplayCueParameters.RawMagnitude = NewDamage;
			// 	UE_LOG(LogTemp, Warning, TEXT("奶瓜GC"))
			// 	// 播放奶瓜数字
			// 	Props.SourceASC->ExecuteGameplayCue(UCAbilitySystemStatics::GetDamageNumberGameplayCueTag(), BlastingGameplayCueParameters);
			// }

			// 接口的方法，失败
			// if (AActor* Target = Data.Target.AbilityActorInfo->AvatarActor.Get())
			// {
			// 	if (Props.SourceCharacter->Implements<UCombatInterface>())
			// 	{
			// 		FNumberPopRequest NumberPopRequest;
			// 		// 设置特效的位置
			// 		NumberPopRequest.WorldLocation = Target->GetActorLocation();
			// 		NumberPopRequest.WorldLocation.Z += 200.f;
			// 		NumberPopRequest.NumberToDisplay = NewDamage;
			// 		NumberPopRequest.bIsCriticalDamage = bCriticalHit;
			// 		ICombatInterface::Execute_AddNiagaraText(Target,NumberPopRequest);
			// 	}
			// }
			
			// Source是输出者，Target是挨打的
			// UE_LOG(LogTemp, Warning, TEXT("SourceASCName: %s"), *Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent()->GetName())
			// UE_LOG(LogTemp, Warning, TEXT("TargetASCName: %s"), *UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Data.Target.AbilityActorInfo->AvatarActor.Get())->GetName())

			// UE_LOG(LogTemp, Warning, TEXT("SourceName: %s"), *Data.EffectSpec.GetContext().GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get()->GetName())
			// UE_LOG(LogTemp, Warning, TEXT("TargetName: %s"), *Data.Target.AbilityActorInfo->AvatarActor.Get()->GetName())
		}
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

void UCAttributeSet::OnRep_AttackPowerCoefficient(const FGameplayAttributeData& OldAttackPowerCoefficient)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, AttackPowerCoefficient, OldAttackPowerCoefficient);
}

void UCAttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCAttributeSet, BaseDamage, OldBaseDamage);
}

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

void UCAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsCriticalHit)
{
	for (int32 i = 0; ;++i)
	{
		if (ACPlayerController* PC = Cast<ACPlayerController>(UGameplayStatics::GetPlayerController(Props.TargetCharacter,i)))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsCriticalHit); //调用显示伤害数字
		}else
		{
			break;
		}
	}
}

void UCAttributeSet::Client_ShowFloatingText_Implementation(const FEffectProperties& Props, const float Damage,
	bool IsCriticalHit)
{
	for (int32 i = 0; ;++i)
	{
		if (ACPlayerController* PC = Cast<ACPlayerController>(UGameplayStatics::GetPlayerController(Props.TargetCharacter,i)))
		{
			// PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsCriticalHit); //调用显示伤害数字
			if (PC->IsLocalController())
			{
				if (Props.TargetCharacter->Implements<UCombatInterface>())
				{
					FNumberPopRequest NumberPopRequest;
					NumberPopRequest.WorldLocation = Props.TargetCharacter->GetActorLocation();
					NumberPopRequest.WorldLocation.Z += 200.f;
					NumberPopRequest.bIsCriticalDamage = IsCriticalHit;
					NumberPopRequest.NumberToDisplay = Damage;
					// NumberPopComponent->AddNumberPop(NumberPopRequest);
					ICombatInterface::Execute_AddNiagaraText(Props.TargetCharacter,NumberPopRequest);
				}
			}
		}else
		{
			break;
		}
	}
}	