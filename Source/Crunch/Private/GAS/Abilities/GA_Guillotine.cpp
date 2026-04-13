// 幻雨喜欢小猫咪


#include "GA_Guillotine.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/TA/TargetActor_Line.h"

UGA_Guillotine::UGA_Guillotine()
{
}

void UGA_Guillotine::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !GuillotineMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放蓄力+劈砍动画
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GuillotineMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UGA_Guillotine::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UGA_Guillotine::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UGA_Guillotine::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UGA_Guillotine::K2_EndAbility);
		PlayMontage->ReadyForActivation();

		// 等待劈砍落地动画事件
		UAbilityTask_WaitGameplayEvent* WaitSlamEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Guillotine_Slam);
		WaitSlamEvent->EventReceived.AddDynamic(this, &UGA_Guillotine::OnSlam);
		WaitSlamEvent->ReadyForActivation();
	}
}

void UGA_Guillotine::OnSlam(FGameplayEventData Payload)
{
	// 创建前方线性检测
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::CustomMulti,
		TargetActorClass
	);

	WaitTargetData->ValidData.AddDynamic(this, &UGA_Guillotine::TargetReceived);
	WaitTargetData->ReadyForActivation();

	// 生成线性检测器
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);

	if (ATargetActor_Line* LineActor = Cast<ATargetActor_Line>(TargetActor))
	{
		// 短距离、宽半径 = 模拟前方扇形劈砍
		LineActor->ConfigureTargetSetting(
			CleaveRange,
			CleaveRadius,
			0.f, // 单次检测，不需要间隔
			GetOwnerTeamId(),
			ShouldDrawDebug()
		);
	}

	WaitTargetData->FinishSpawningActor(this, TargetActor);

	// 播放劈砍特效 + 相机震动
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		OwnerASC->ExecuteGameplayCue(GuillotineCueTag, CueParams);
		OwnerASC->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), CueParams);
	}
}

void UGA_Guillotine::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_HasAuthority())
	{
		return;
	}

	const int32 AbilityLevel = GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo);

	// 对所有目标应用基础伤害
	ApplyDamageToTargetDataHandle(TargetDataHandle, DamageEffect, AbilityLevel);

	// 对每个目标检查斩杀条件和流血
	const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, 0);
	for (AActor* Target : Targets)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC)
		{
			continue;
		}

		// 斩杀检测：低血量目标额外伤害
		bool bHasHealth = false;
		float CurrentHealth = TargetASC->GetGameplayAttributeValue(UCAttributeSet::GetHealthAttribute(), bHasHealth);
		float MaxHealth = 0.f;
		bool bHasMaxHealth = false;
		MaxHealth = TargetASC->GetGameplayAttributeValue(UCAttributeSet::GetMaxHealthAttribute(), bHasMaxHealth);

		if (bHasHealth && bHasMaxHealth && MaxHealth > 0.f)
		{
			float HealthPercent = CurrentHealth / MaxHealth;
			if (HealthPercent <= ExecuteHealthThreshold && HealthPercent > 0.f)
			{
				// 触发斩杀额外伤害
				ApplyDamageToActor(Target, ExecuteBonusDamage, AbilityLevel);
			}
		}

		// 施加流血DOT
		if (BleedEffect)
		{
			FGameplayEffectSpecHandle BleedSpec = MakeOutgoingGameplayEffectSpec(BleedEffect, AbilityLevel);
			if (BleedSpec.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*BleedSpec.Data.Get());
			}
		}
	}
}
