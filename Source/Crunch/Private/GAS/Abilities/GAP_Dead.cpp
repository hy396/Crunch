// 幻雨喜欢小猫咪


#include "GAS/Abilities/GAP_Dead.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"
#include "GAS/Core/TGameplayTags.h"

UGAP_Dead::UGAP_Dead()
{
	// 设置网络执行策略为仅在服务器端执行
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// 创建一个新触发数据对象
	FAbilityTriggerData TriggerData;

	// 设置触发数据的触发源为游戏事件
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = TGameplayTags::Stats_Dead;

	AbilityTriggers.Add(TriggerData);

	// 死亡去除眩晕状态
	ActivationBlockedTags.RemoveTag(TGameplayTags::Stats_Stun);
}

// void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
// {
// 	if (K2_HasAuthority())
// 	{
// 		// MMC 获取
// 		// AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
// 		// ECC 获取
// 		AActor* Killer = nullptr;
// 		// if (TriggerEventData->ContextHandle.GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get())
// 		// {
// 		// 	Killer = TriggerEventData->ContextHandle.GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get();
// 		// 	// UE_LOG(LogTemp, Warning, TEXT("TriggerEventData->ContextHandle.GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get(): %s"), *Killer->GetName())
// 		// }
// 		if (TriggerEventData->Target)
// 		{
// 			Killer = static_cast<AActor*>(TriggerEventData->Target);
// 			// Killer = const_cast<AActor*>(TriggerEventData->Target.Get());
// 			// UE_LOG(LogTemp, Warning, TEXT("TriggerEventData->Target: %s"), *Killer->GetName())
// 		}
//
// 		
// 		// 击杀者不存在或者击杀者为非英雄单位
// 		if (!Killer || !UCAbilitySystemStatics::IsHero(Killer))
// 		{
// 			Killer = nullptr;
// 		}
//
// 		// 判断死亡单位是英雄还是小兵
// 		AActor* DeadActor = GetAvatarActorFromActorInfo();
// 		bool bIsHero = UCAbilitySystemStatics::IsHero(DeadActor);
// 		if (bIsHero)
// 		{
// 			// 被击杀者是英雄单位
// 			// 给击杀者赋予击杀英雄GE
// 			FGameplayEffectSpecHandle KillHeroEffectSpec = MakeOutgoingGameplayEffectSpec(KillHeroEffect);
// 			K2_ApplyGameplayEffectSpecToTarget(KillHeroEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
// 			// 给被击杀者赋予死亡GE
// 			FGameplayEffectSpecHandle DeadEffectSpec = MakeOutgoingGameplayEffectSpec(DeadEffect);
// 			K2_ApplyGameplayEffectSpecToTarget(DeadEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(DeadActor));
// 		}else{
// 			// 被击杀者不是英雄单位（小兵）
// 			// 给击杀者赋予补兵GE
// 			FGameplayEffectSpecHandle LastHitEffectSpec = MakeOutgoingGameplayEffectSpec(LastHitEffect);
// 			K2_ApplyGameplayEffectSpecToTarget(LastHitEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
// 		}
//
// 		
// 		// 获取需要奖励的目标
// 		TArray<AActor*> RewardTargets = GetRewardTargets();
// 		// 如果没有奖励目标又没有击杀者直接结束技能不需要奖励
// 		if (RewardTargets.Num() == 0 && !Killer)
// 		{
// 			K2_EndAbility();
// 			return;
// 		}
//
// 		// 击杀者存在并且不在奖励目标中，将击杀者添加进去
// 		if (Killer && !RewardTargets.Contains(Killer))
// 		{
// 			RewardTargets.Add(Killer);
// 		}
// 		bool bFound = false;
// 		// 获取角色当前经验属性值
// 		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
//
// 		// 计算总奖励（基础奖励+基于经验的奖励）
// 		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
// 		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;
//
// 		// 判断是否有击杀的英雄单位，给他分大头
// 		if (Killer)
// 		{
// 			float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
// 			float KillerGoldReward = TotalGoldReward * KillerRewardPortion;
// 			// UE_LOG(LogTemp, Warning, TEXT("击杀者经验奖励：%f"), KillerExperienceReward)
// 			// 创建击杀者奖励效果
// 			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
// 			// 设置GE上面的属性值
// 			EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, KillerExperienceReward);
// 			EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, KillerGoldReward);
//
// 			// 对击杀者应用奖励效果
// 			K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
//
// 			// 从总奖励中扣除击杀者部分
// 			TotalExperienceReward -= KillerExperienceReward;
// 			TotalGoldReward -= KillerGoldReward;
// 		}
// 		// 把剩余的奖励平均分配给没有k到头的英雄单位
// 		float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();
// 		float GoldPerTarget = TotalGoldReward / RewardTargets.Num();
// 		// UE_LOG(LogTemp, Warning, TEXT("数组里面多少人%d"), RewardTargets.Num())
// 		// UE_LOG(LogTemp, Warning, TEXT("平均经验奖励：%f"), ExperiencePerTarget)
//
// 		// 创建群体奖励效果
// 		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
// 		// 设置GE上面的属性值
// 		EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, ExperiencePerTarget);
// 		EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, GoldPerTarget);
// 		
// 		// 对所有奖励目标应用奖励
// 		K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
// 		K2_EndAbility();
// 	}
// }
void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		// 从事件数据中读取击杀信息
		AActor* Killer = nullptr;
		TArray<AActor*> AssistHeroes;
		
		if (TriggerEventData && TriggerEventData->OptionalObject)
		{
			const UDeadEventPayload* DeadPayload = Cast<UDeadEventPayload>(TriggerEventData->OptionalObject);
			if (DeadPayload)
			{
				Killer = DeadPayload->Killer.Get();
				for (const TObjectPtr<AActor>& AssistHero : DeadPayload->AssistHeroes)
				{
					if (AssistHero)
					{
						AssistHeroes.Add(AssistHero.Get());
					}
				}
			}
		}

		// 如果没有从新方式获取到击杀者，尝试从旧方式获取（兼容性）
		if (!Killer && TriggerEventData && TriggerEventData->Target)
		{
			Killer = static_cast<AActor*>(TriggerEventData->Target);
			// 击杀者不存在或者击杀者为非英雄单位
			if (!Killer || !UCAbilitySystemStatics::IsHero(Killer))
			{
				Killer = nullptr;
			}
		}

		// 判断死亡单位是英雄还是小兵
		AActor* DeadActor = GetAvatarActorFromActorInfo();
		const bool bIsHero = UCAbilitySystemStatics::IsHero(DeadActor);
		
		// 奖励金额
		// float TotalGoldReward = 0.f;
		// float TotalExperienceReward = 0.f;

		if (bIsHero)
		{
			// ============ 被击杀者是英雄单位 ============
			
			// 获取被击杀者的属性集以计算赏金
			float KillStreak = 0.f;
			float DeathStreak = 0.f;
			
			if (UAbilitySystemComponent* DeadASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DeadActor))
			{
				bool bFound = false;
				KillStreak = DeadASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetKillStreakAttribute(), bFound);
				DeathStreak = DeadASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetDeathStreakAttribute(), bFound);
			}

			// 步骤 A - 计算当前人头价（基于连杀和连败）
			float RawBounty = BaseBounty + (KillStreak * BountyPerStreak) - (DeathStreak * PenaltyPerDeath);
			float TotalGoldReward = FMath::Clamp(RawBounty, MinBounty, MaxBounty);

			// 经验值计算保持不变（基于被击杀者当前经验）
			bool bFound = false;
			float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
			float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;

			// 给击杀者赋予击杀英雄GE
			if (Killer)
			{
				// 应用击杀英雄的GE
				if(KillHeroEffect)
				{
					FGameplayEffectSpecHandle KillHeroEffectSpec = MakeOutgoingGameplayEffectSpec(KillHeroEffect);
					K2_ApplyGameplayEffectSpecToTarget(KillHeroEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
				}

				// 额外应用连杀 GE
				if(KillStreakIncreaseEffect)
				{
					FGameplayEffectSpecHandle KillStreakSpec = MakeOutgoingGameplayEffectSpec(KillStreakIncreaseEffect);
					K2_ApplyGameplayEffectSpecToTarget(KillStreakSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
				}
				
				// 给击杀者发放奖励：全部赏金 + 全部经验
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
				EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, TotalGoldReward);
				EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, TotalExperienceReward);
				K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
			}
			if(DeadEffect)
			{
				// 给被击杀者赋予死亡GE
				FGameplayEffectSpecHandle DeadEffectSpec = MakeOutgoingGameplayEffectSpec(DeadEffect);
				K2_ApplyGameplayEffectSpecToTarget(DeadEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(DeadActor));
			}
			
			if(DeathStreakIncreaseEffect)
			{
				// 额外应用连败 GE
				FGameplayEffectSpecHandle DeathStreakSpec = MakeOutgoingGameplayEffectSpec(DeathStreakIncreaseEffect);
				K2_ApplyGameplayEffectSpecToTarget(DeathStreakSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(DeadActor));
			}
			
			// TODO: 给助攻者增加助攻计数
			// for (AActor* AssistHero : AssistHeroes)
			// {
			// 	if (AssistHero)
			// 	{
			// 		// TODO: 实现助攻奖励（可以创建专门的助攻GE）
			// 		FGameplayEffectSpecHandle AssistHeroEffectSpec = MakeOutgoingGameplayEffectSpec(AssistEffect);
			// 		K2_ApplyGameplayEffectSpecToTarget(AssistHeroEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(AssistHero));
			// 		K2_ApplyGameplayEffectSpecToTarget(AssistHeroEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(AssistHeroes, true));
			// 	}
			// }
			if(AssistHeroes.Num() > 0){
				// 应用助攻
				if(AssistEffect){
					FGameplayEffectSpecHandle AssistHeroEffectSpec = MakeOutgoingGameplayEffectSpec(AssistEffect);
					K2_ApplyGameplayEffectSpecToTarget(AssistHeroEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(AssistHeroes, true));
				}
				if(RewardEffect){
					// KillerRewardPortion
					// 助攻奖励, 50%
					// float AssistGoldReward = TotalGoldReward / 2.0f;
					// float AssistExperienceReward = TotalExperienceReward / 2.0f;

					float AssistGoldReward = TotalGoldReward * KillerRewardPortion;
					float AssistExperienceReward = TotalExperienceReward * KillerRewardPortion;

					FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
					EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, AssistGoldReward);
					EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, AssistExperienceReward);
					// 批量应用助攻奖励
					K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(AssistHeroes, true));
				}
			}
			
			// ============ 连杀/连败状态更新提示 ============
			// 注意：这些更新应该通过 GE 来实现，以确保网络同步和数据一致性
			// 建议在 KillHeroEffect GE 中添加对 KillStreak 属性的 +1 修改
			// 建议在 DeadEffect GE 中添加对 DeathStreak 属性的 +1 和 KillStreak 的清空修改
		}
		else
		{
			// ============ 被击杀者不是英雄单位（小兵） ============
			// 保持原有的固定奖励逻辑, 不受连杀/连败机制影响, 给范围内的队友分配奖励
			
			bool bFound = false;
			float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);

			// 奖励总额
			float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;
			float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;

			// 获取需要奖励的目标（附近的队友）
			TArray<AActor*> RewardTargets = GetRewardTargets();
			
			// // 如果没有奖励目标又没有击杀者直接结束技能
			// if (RewardTargets.Num() == 0 && !Killer)
			// {
			// 	K2_EndAbility();
			// 	return;
			// }
			//
			// // 击杀者存在并且不在奖励目标中，将击杀者添加进去
			// if (Killer && !RewardTargets.Contains(Killer))
			// {
			// 	RewardTargets.Add(Killer);
			// }
			// ================== 计算 Killer 奖励 ==================

			// 计算并分配奖励
			if (Killer)
			{
				// 移除击杀者
				if (RewardTargets.Contains(Killer))
					RewardTargets.Remove(Killer);
				
				// 给击杀者分配奖励
				float KillerGoldReward = TotalGoldReward * KillerRewardPortion;
				float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
				// 从中再分一杯羹
				const int32 ExtraShareCount = RewardTargets.Num() + 1;
				KillerGoldReward += (TotalGoldReward - KillerGoldReward) / ExtraShareCount;
				KillerExperienceReward  += (TotalExperienceReward - KillerExperienceReward) / ExtraShareCount;
				if(RewardEffect)
				{
					// 给击杀者发放奖励
					FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
					EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, KillerGoldReward);
					EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, KillerExperienceReward);
					K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
				}				
				// 从总奖励中扣除击杀者部分
				TotalGoldReward -= KillerGoldReward;
				TotalExperienceReward -= KillerExperienceReward;
				// 给击杀者赋予补兵GE
				if(LastHitEffect)
				{
					FGameplayEffectSpecHandle LastHitEffectSpec = MakeOutgoingGameplayEffectSpec(LastHitEffect);
					K2_ApplyGameplayEffectSpecToTarget(LastHitEffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
				}
			}
			
			// 剩余奖励平均分配给范围内的队友
			if (RewardTargets.Num() > 0)
			{
				float GoldPerTarget = TotalGoldReward / RewardTargets.Num();
				float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();

				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
				EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, GoldPerTarget);
				EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, ExperiencePerTarget);
				
				// 批量应用奖励
				K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
			}
		}
		K2_EndAbility();
	}
}



TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	TSet<AActor*> OutActors;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		return OutActors.Array();
	}

	// 配置碰撞检测参数
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);  // 使用奖励范围作为检测半径

	TArray<FOverlapResult> OverlapResults;
	// 检测碰撞
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			// 获取团队接口，给死亡目标的敌对单位提供奖励
			const IGenericTeamAgentInterface* TeamInterface =  Cast<IGenericTeamAgentInterface>(OverlapResult.GetActor());
			// 过滤非敌对单位
			if (!TeamInterface || TeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
			{
				continue;
			}

			// 判断是否为英雄单位
			if (!UCAbilitySystemStatics::IsHero(OverlapResult.GetActor()))
			{
				continue;
			}
			
			// 添加到奖励目标集合
			OutActors.Add(OverlapResult.GetActor());
		}
	}
	return OutActors.Array();
}
