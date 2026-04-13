// 幻雨喜欢小猫咪


#include "GA_Earthquake.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/TA/TargetActor_Around.h"

UGA_Earthquake::UGA_Earthquake()
{
	// 释放期间无敌 + 控制免疫
	ActivationOwnedTags.AddTag(TGameplayTags::Status::Invincible);
	ActivationOwnedTags.AddTag(TGameplayTags::Status::CrowdControlImmunity);
}

void UGA_Earthquake::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !EarthquakeMontage)
	{
		K2_EndAbility();
		return;
	}

	CurrentSlamIndex = 0;

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 起跳
		PushSelf(FVector(0.f, 0.f, LeapUpSpeed));

		// 播放地裂完整动画
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, EarthquakeMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UGA_Earthquake::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UGA_Earthquake::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UGA_Earthquake::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UGA_Earthquake::K2_EndAbility);
		PlayMontage->ReadyForActivation();

		// 监听三次砸地事件
		UAbilityTask_WaitGameplayEvent* Wait1 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Earthquake_Slam1);
		Wait1->EventReceived.AddDynamic(this, &UGA_Earthquake::OnSlam1);
		Wait1->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* Wait2 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Earthquake_Slam2);
		Wait2->EventReceived.AddDynamic(this, &UGA_Earthquake::OnSlam2);
		Wait2->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* Wait3 = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Earthquake_Slam3);
		Wait3->EventReceived.AddDynamic(this, &UGA_Earthquake::OnSlam3);
		Wait3->ReadyForActivation();
	}
}

void UGA_Earthquake::OnSlam1(FGameplayEventData Payload)
{
	ExecuteSlam(Slam1Radius, Slam1DamageEffect, StunEffect, false);
}

void UGA_Earthquake::OnSlam2(FGameplayEventData Payload)
{
	ExecuteSlam(Slam2Radius, Slam2DamageEffect, StunEffect, false);
}

void UGA_Earthquake::OnSlam3(FGameplayEventData Payload)
{
	ExecuteSlam(Slam3Radius, Slam3DamageEffect, KnockupEffect, true);
}

void UGA_Earthquake::ExecuteSlam(float Radius, const FGenericDamageEffectDef& DamageEffect,
                                  TSubclassOf<UGameplayEffect> CCEffect, bool bPushTargets)
{
	// 缓存当前砸地参数（因为TargetReceived是异步回调）
	CurrentSlamDamage = DamageEffect;
	CurrentCCEffect = CCEffect;
	bCurrentSlamPushTargets = bPushTargets;

	// 创建范围检测
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &UGA_Earthquake::OnSlamTargetReceived);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	if (ATargetActor_Around* AroundActor = Cast<ATargetActor_Around>(TargetActor))
	{
		AroundActor->ConfigureDetection(Radius, GetOwnerTeamId(), EarthquakeCueTag);
	}
	WaitTargetData->FinishSpawningActor(this, TargetActor);

	// 附加到角色位置
	if (TargetActor)
	{
		TargetActor->AttachToComponent(
			GetOwningComponentFromActorInfo(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
		);
	}

	// 播放砸地特效 + 相机震动
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		CueParams.RawMagnitude = Radius;
		OwnerASC->ExecuteGameplayCue(EarthquakeCueTag, CueParams);
		OwnerASC->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), CueParams);
	}
}

void UGA_Earthquake::OnSlamTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_HasAuthority())
	{
		return;
	}

	const int32 AbilityLevel = GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo);

	// 应用伤害
	ApplyDamageToTargetDataHandle(TargetDataHandle, CurrentSlamDamage, AbilityLevel);

	// 应用CC效果（眩晕/击飞）
	if (CurrentCCEffect)
	{
		const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, 0);
		for (AActor* Target : Targets)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			if (IsValid(TargetASC))
			{
				FGameplayEffectSpecHandle CCSpec = MakeOutgoingGameplayEffectSpec(CurrentCCEffect, AbilityLevel);
				if (CCSpec.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*CCSpec.Data.Get());
				}
			}
		}
	}

	// 第3次砸地：击退
	if (bCurrentSlamPushTargets)
	{
		PushTargetsFromOwnerLocation(TargetDataHandle, Slam3PushSpeed);
	}
}
