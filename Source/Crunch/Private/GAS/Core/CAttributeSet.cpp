// 幻雨喜欢小猫咪


#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
// #include "Character/Interaction/CombatInterface.h"
#include "TGameplayTags.h"
#include "Framework/CGameMode.h"
#include "Framework/CGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CPlayerController.h"
#include "Player/MPlayerState.h"
#include "Player/CPlayerCharacter.h"

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

void UCAttributeSet::UpdateKillAndDeathStreaks(UAbilitySystemComponent* KillerASC, UAbilitySystemComponent* DeadASC)
{
    // 更新击杀者的连杀
    if (KillerASC)
    {
        bool bFound = false;
        float KillStreak = KillerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetKillStreakAttribute(), bFound);
        if (bFound)
        {
            KillerASC->SetNumericAttributeBase(UCHeroAttributeSet::GetKillStreakAttribute(), KillStreak + 1.f);
        }
        
        // 清除击杀者的连败
        KillerASC->SetNumericAttributeBase(UCHeroAttributeSet::GetDeathStreakAttribute(), 0.f);
    }
    
    // 更新死亡者的连败
    if (DeadASC)
    {
        bool bFound = false;
        float DeathStreak = DeadASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetDeathStreakAttribute(), bFound);
        if (bFound)
        {
            DeadASC->SetNumericAttributeBase(UCHeroAttributeSet::GetDeathStreakAttribute(), DeathStreak + 1.f);
        }
        
        // 清除死亡者的连杀
        DeadASC->SetNumericAttributeBase(UCHeroAttributeSet::GetKillStreakAttribute(), 0.f);
    }
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
	// Props.SourceAvatarActor
	// 记录最近的伤害来源及时间（只统计来自英雄角色的伤害）
	if (Props.SourceAvatarActor && GetOwningActor() && Props.SourceAvatarActor != GetOwningActor() && Props.SourceAvatarActor->IsA<ACPlayerCharacter>())
	{
		RecentDamageSourcesMap.Add(Props.SourceCharacter, GetWorld()->GetTimeSeconds());
	}
	// if (Props.SourceAvatarActor && Props.SourceAvatarActor != Props.TargetAvatarActor && Props.SourceAvatarActor->IsA<ACPlayerCharacter>())
	// {
	// 	RecentDamageSourcesMap.Add(Props.SourceCharacter, GetWorld()->GetTimeSeconds());
	// }
	
	const float NewHealth = GetHealth() - NewDamage;
	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
	// UE_LOG(LogTemp, Log, TEXT("NewDamage: %f"), NewDamage)
			
	// 如果生命小于等于0触发死亡
	if (NewHealth <= 0.f)
	{
		// TODO: 分发助攻给他们
		
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
		PC->Client_ShowDamageNumber(Damage, Props.TargetCharacter, IsCriticalHit, DamageType); //调用显示伤害数字
	}
	// 从目标身上获取PC并显示伤害数字
	if(ACPlayerController* PC = Cast<ACPlayerController>(Props.TargetCharacter->Controller))
	{
		PC->Client_ShowDamageNumber(Damage, Props.TargetCharacter, IsCriticalHit, DamageType); //调用显示伤害数字
	}
}

// void UCAttributeSet::OnDeadAbility(const FEffectProperties& Props)
// {
// 	FGameplayEventData DeadAbilityEventData;
// 	if (Props.SourceAvatarActor && GetOwningActor())
// 	{
// 		// UE_LOG(LogTemp, Warning, TEXT("Dead：%s"), *GetOwningActor()->GetName())
// 		DeadAbilityEventData.Target = Props.SourceAvatarActor;

// 		// ----------------------------分发助攻？？--------------------------------
// 		// 被击杀单位是否为英雄
// 		if (UCAbilitySystemStatics::IsHero(GetOwningActor()))
// 		{
// 			// 最后击杀单位为小兵，将判断谁是最后一个攻击该单位的英雄
// 			float LastDamageTime = -1.f;
// 			AActor* LastDamageSource = nullptr;
// 			// 添加助攻的英雄单位
// 			TArray<AActor*> AssistHeroes;
// 			for (auto It = RecentDamageSourcesMap.CreateIterator(); It; ++It)
// 			{
// 				AActor* Damager = It.Key().Get();
// 				float DamageTime = It.Value();
// 				// 如果伤害来源记录时间超过一定时间（例如30秒），则移除该记录
// 				if (GetWorld()->GetTimeSeconds() - DamageTime > AssistTimeThreshold)
// 				{
// 					continue;
// 				}
// 				if (Damager)
// 				{
// 					// 寻找造成伤害时间最晚的英雄（作为击杀者）
// 					// 直接比较时间戳大小，找出最大的那个（最接近现在的）
// 					if (DamageTime > LastDamageTime)
// 					{
// 						LastDamageTime = DamageTime;
// 						LastDamageSource = Damager;
// 					}
// 					// 记录所有符合条件的英雄到助攻列表（用于去重给助攻）
// 					AssistHeroes.AddUnique(Damager);
// 				}
// 			}
// 			// 此人就是最终击杀者，给他加个人头
// 			if (LastDamageSource)
// 			{
// 				// TODO: 给 LastDamageSource 加人头

// 				// TODO: AssistHeroes中移除最后伤害者为其他人添加助攻
// 				for (auto AssistHero : AssistHeroes)
// 				{
// 					if (AssistHero != LastDamageSource)
// 					{
// 						// TODO: 添加助攻的单位
							
// 					}
// 				}
// 			}
// 		}		
		
	
// 		// -----------------------------------------------------------------------
// 	}

// 	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), 
// 		TGameplayTags::Stats_Dead, 
// 		DeadAbilityEventData);
// }

void UCAttributeSet::OnDeadAbility(const FEffectProperties& Props)
{
    FGameplayEventData DeadAbilityEventData;
    
    if (Props.SourceAvatarActor && GetOwningActor())
    {
        DeadAbilityEventData.Target = Props.SourceAvatarActor;

		// 创建击杀事件数据对象
        UDeadEventPayload* DeadPayload = NewObject<UDeadEventPayload>();
        DeadPayload->Killer = nullptr;

        // 分发助攻 - 被击杀单位是否为英雄
        if (UCAbilitySystemStatics::IsHero(GetOwningActor()))
        {
            // 最后击杀单位为小兵，将判断谁是最后一个攻击该单位的英雄
            float LastDamageTime = -1.f;
            AActor* LastDamageSource = nullptr;
            TArray<AActor*> AssistHeroes;
            
            for (auto It = RecentDamageSourcesMap.CreateIterator(); It; ++It)
            {
                AActor* Damager = It.Key().Get();
                float DamageTime = It.Value();
                
                // 如果伤害来源记录时间超过一定时间，则移除该记录
                if (GetWorld()->GetTimeSeconds() - DamageTime > AssistTimeThreshold)
                {
                    continue;
                }
                
                if (Damager)
                {
                    // 寻找造成伤害时间最晚的英雄（作为击杀者）
                    if (DamageTime > LastDamageTime)
                    {
                        LastDamageTime = DamageTime;
                        LastDamageSource = Damager;
                    }
                    // 记录所有符合条件的英雄到助攻列表
                    AssistHeroes.AddUnique(Damager);
                }
            }
            
            // 设置击杀者和助攻者
            if (LastDamageSource)
            {
                // 有英雄造成伤害，最后一个攻击的英雄是击杀者
                DeadPayload->Killer = LastDamageSource;
                
                // 设置助攻者（从助攻列表中移除击杀者）
                for (AActor* AssistHero : AssistHeroes)
                {
                    if (AssistHero != LastDamageSource)
                    {
                        DeadPayload->AssistHeroes.Add(AssistHero);
                    }
                }
				// 拥有击杀者的英雄单位
            	if (ACGameMode* Gm = GetWorld()->GetAuthGameMode<ACGameMode>())
            	{
            		// 添加击杀
            		if (const ACPlayerCharacter* PC = Cast<ACPlayerCharacter>(LastDamageSource))
            		{
            			// 添加团队击杀
            			Gm->AddPlayerKillForTeam(PC->GetGenericTeamId());
            			/*
            			 * 下面的方案直接赋值可能不是很好，我将决定移入GAP_Dead 中 使用GE实现
						if (UAbilitySystemComponent* KillAsc = PC->GetAbilitySystemComponent())
						{
							// 获取到该英雄单位的连续击杀数量
							bool bFound = false;
							const float KillStreak = KillAsc->GetGameplayAttributeValue(
								UCHeroAttributeSet::GetKillStreakAttribute(), bFound);
							// 为击杀单位添加连杀
							if (bFound)
								KillAsc->SetNumericAttributeBase(UCHeroAttributeSet::GetKillStreakAttribute(), KillStreak + 1.f);
							// 清除该英雄的连续死亡
							bFound = false;
							KillAsc->GetGameplayAttributeValue(
								UCHeroAttributeSet::GetDeathStreakAttribute(), bFound);
							if (bFound)
								KillAsc->SetNumericAttributeBase(UCHeroAttributeSet::GetDeathStreakAttribute(), 0.f);
						}
            			// 清空死亡单位的连续击杀，并将连续死亡加一
            			if (UAbilitySystemComponent* TargetAsc = Props.TargetASC ? Props.TargetASC : GetOwningAbilitySystemComponent())
            			{
            				bool bFound = false;
            				const float DeathStreak = TargetAsc->GetGameplayAttributeValue(
									UCHeroAttributeSet::GetDeathStreakAttribute(), bFound);
            				if (bFound)
            					TargetAsc->SetNumericAttributeBase(UCHeroAttributeSet::GetDeathStreakAttribute(), DeathStreak + 1.f);
					       
            				bFound = false;
            				TargetAsc->GetGameplayAttributeValue(
								UCHeroAttributeSet::GetKillStreakAttribute(), bFound);
            				if (bFound)
            					TargetAsc->SetNumericAttributeBase(UCHeroAttributeSet::GetKillStreakAttribute(), 0.f);	
            			}
            			*/
            			// 下面的和上面的是一样的
            			// if (UAbilitySystemComponent* KillAsc =  PC->GetAbilitySystemComponent() , UAbilitySystemComponent* TargetAsc = Props.TargetASC ? Props.TargetASC : GetOwningAbilitySystemComponent(); KillAsc && TargetAsc)
            			// {
            			// 	UpdateKillAndDeathStreaks(KillAsc, TargetAsc);
            			// }
            		}
            	}

            	// 广播击杀事件到GameState
            	if (ACGameState* GameState = GetWorld()->GetGameState<ACGameState>())
            	{
            		// 获取击杀者、被击杀者和助攻者的PlayerState
					ACPlayerCharacter* KillerChar = Cast<ACPlayerCharacter>(LastDamageSource);
					AMPlayerState* KillerState = KillerChar ? KillerChar->GetPlayerState<AMPlayerState>() : nullptr;

					ACPlayerCharacter* VictimChar = Cast<ACPlayerCharacter>(GetOwningActor());
					AMPlayerState* VictimState = VictimChar ? VictimChar->GetPlayerState<AMPlayerState>() : nullptr;

					TArray<AMPlayerState*> AssistStates;
					for (AActor* AssistHero : DeadPayload->AssistHeroes)
					{
					    ACPlayerCharacter* AssistChar = Cast<ACPlayerCharacter>(AssistHero);
					    if (AMPlayerState* AssistState = AssistChar ? AssistChar->GetPlayerState<AMPlayerState>() : nullptr)
					    {
					        AssistStates.Add(AssistState);
					    }
					}

            		// 广播击杀事件
            		// GameState->Server_NotifyPlayerKilled(KillerState, VictimState, AssistStates);
            		// 添加击杀
            		GameState->Multicast_OnPlayerKilled(KillerState, VictimState, AssistStates);
            	}
            }
            else
            {
                // 没有任何英雄造成伤害，可能是被小兵、野怪或环境伤害击杀
                // 这种情况下，伤害源就是击杀者
                DeadPayload->Killer = Props.SourceAvatarActor;
                
                // 此时没有助攻者，因为只有英雄才能获得助攻
            }
        }else{
			// 被击杀者不是英雄（小兵、野怪等）
            // 简单处理：伤害源就是击杀者，没有助攻
            DeadPayload->Killer = Props.SourceAvatarActor;
		}
		// 将击杀信息放入事件数据的 OptionalObject 中
        DeadAbilityEventData.OptionalObject = DeadPayload;
    }

    // 发送死亡事件
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), 
        TGameplayTags::Stats_Dead, 
        DeadAbilityEventData);
}
