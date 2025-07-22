// 幻雨喜欢小猫咪


#include "GAS/Abilities/GAP_Dead.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "GAS/Core/CAbilitySystemStatics.h"
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

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		// MMC 获取
		// AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		// ECC 获取
		AActor* Killer = nullptr;
		if (TriggerEventData->ContextHandle.GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get())
		{
			Killer = TriggerEventData->ContextHandle.GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get();
			UE_LOG(LogTemp, Warning, TEXT("TriggerEventData->ContextHandle.GetOriginalInstigatorAbilitySystemComponent()->AbilityActorInfo->AvatarActor.Get(): %s"), *Killer->GetName())
		}
		if (TriggerEventData->Target)
		{
			Killer = static_cast<AActor*>(TriggerEventData->Target);
			UE_LOG(LogTemp, Warning, TEXT("TriggerEventData->Target: %s"), *Killer->GetName())
		}

		
		// 击杀者不存在或者击杀者为非英雄单位
		if (!Killer || !UCAbilitySystemStatics::IsHero(Killer))
		{
			Killer = nullptr;
		}
		// 获取需要奖励的目标
		TArray<AActor*> RewardTargets = GetRewardTargets();
		// 如果没有奖励目标又没有击杀者直接结束技能不需要奖励
		if (RewardTargets.Num() == 0 && !Killer)
		{
			K2_EndAbility();
			return;
		}

		// 击杀者存在并且不在奖励目标中，将击杀者添加进去
		if (Killer && !RewardTargets.Contains(Killer))
		{
			RewardTargets.Add(Killer);
		}
		bool bFound = false;
		// 获取角色当前经验属性值
		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);

		// 计算总奖励（基础奖励+基于经验的奖励）
		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;

		// 判断是否有击杀的英雄单位，给他分大头
		if (Killer)
		{
			float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
			float KillerGoldReward = TotalGoldReward * KillerRewardPortion;
			UE_LOG(LogTemp, Warning, TEXT("击杀者经验奖励：%f"), KillerExperienceReward)
			// 创建击杀者奖励效果
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
			// 设置GE上面的属性值
			EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, KillerExperienceReward);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, KillerGoldReward);

			// 对击杀者应用奖励效果
			K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));

			// 从总奖励中扣除击杀者部分
			TotalExperienceReward -= KillerExperienceReward;
			TotalGoldReward -= KillerGoldReward;
		}
		// 把剩余的奖励平均分配给没有k到头的英雄单位
		float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();
		float GoldPerTarget = TotalGoldReward / RewardTargets.Num();
		UE_LOG(LogTemp, Warning, TEXT("数组里面多少人%d"), RewardTargets.Num())
		UE_LOG(LogTemp, Warning, TEXT("平均经验奖励：%f"), ExperiencePerTarget)

		// 创建群体奖励效果
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
		// 设置GE上面的属性值
		EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Experience, ExperiencePerTarget);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_Gold, GoldPerTarget);
		
		// 对所有奖励目标应用奖励
		K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
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
