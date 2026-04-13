// 幻雨喜欢小猫咪


#include "GA_ArcaneStorm.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/TA/TargetActor_GroundPick.h"
#include "GAS/TA/TargetActor_BlackHole.h"

UGA_ArcaneStorm::UGA_ArcaneStorm()
{
}

void UGA_ArcaneStorm::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	// 播放瞄准动画（延迟CommitAbility）
	TargetingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	TargetingMontageTask->OnBlendOut.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
	TargetingMontageTask->OnCancelled.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
	TargetingMontageTask->OnInterrupted.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
	TargetingMontageTask->OnCompleted.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
	TargetingMontageTask->ReadyForActivation();

	// 地面目标选择
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &UGA_ArcaneStorm::OnTargetConfirmed);
	WaitTargetData->Cancelled.AddDynamic(this, &UGA_ArcaneStorm::OnTargetCancelled);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	if (ATargetActor_GroundPick* GroundPick = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		GroundPick->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPick->SetTargetAreaRadius(StormRadius);
		GroundPick->SetTargetTraceRange(CastRange);
	}
	WaitTargetData->FinishSpawningActor(this, TargetActor);

	AddAimEffect();
}

void UGA_ArcaneStorm::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAimEffect();

	// 移除自身减速
	if (SelfSlowEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(SelfSlowEffectHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ArcaneStorm::OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	RemoveAimEffect();

	// 解除瞄准动画绑定
	if (TargetingMontageTask)
	{
		TargetingMontageTask->OnBlendOut.RemoveAll(this);
		TargetingMontageTask->OnCancelled.RemoveAll(this);
		TargetingMontageTask->OnInterrupted.RemoveAll(this);
		TargetingMontageTask->OnCompleted.RemoveAll(this);
	}

	// 播放引导动画
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayChannelMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChannelMontage);
		PlayChannelMontage->OnBlendOut.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
		PlayChannelMontage->OnCancelled.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
		PlayChannelMontage->OnInterrupted.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
		PlayChannelMontage->OnCompleted.AddDynamic(this, &UGA_ArcaneStorm::K2_EndAbility);
		PlayChannelMontage->ReadyForActivation();
	}

	// 应用自身减速
	if (SelfSlowEffect)
	{
		SelfSlowEffectHandle = BP_ApplyGameplayEffectToOwner(SelfSlowEffect);
	}

	// 缓存风暴位置
	StormLocation = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0).ImpactPoint;

	// 播放风暴持续特效
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = StormLocation;
		CueParams.RawMagnitude = StormRadius;
		OwnerASC->ExecuteGameplayCue(StormCueTag, CueParams);
	}

	// 启动风暴区域检测（复用BlackHole的TargetActor，PullSpeed=0只做检测）
	StormTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, StormTargetActorClass);
	// 周期性Tick伤害
	StormTargetingTask->ValidData.AddDynamic(this, &UGA_ArcaneStorm::OnStormTick);
	// 风暴结束时爆发
	StormTargetingTask->Cancelled.AddDynamic(this, &UGA_ArcaneStorm::OnStormEnd);
	StormTargetingTask->ReadyForActivation();

	// 生成风暴检测器
	AGameplayAbilityTargetActor* StormActor;
	StormTargetingTask->BeginSpawningActor(this, StormTargetActorClass, StormActor);
	if (ATargetActor_BlackHole* BlackHoleActor = Cast<ATargetActor_BlackHole>(StormActor))
	{
		// PullSpeed=0：不吸引目标，只做范围检测
		BlackHoleActor->ConfigureBlackHole(StormRadius, 0.f, TickInterval, StormDuration, GetOwnerTeamId());
	}
	StormTargetingTask->FinishSpawningActor(this, StormActor);

	// 设置风暴位置
	if (StormActor)
	{
		StormActor->SetActorLocation(StormLocation);
	}
}

void UGA_ArcaneStorm::OnTargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_ArcaneStorm::OnStormTick(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_HasAuthority())
	{
		return;
	}

	const int32 AbilityLevel = GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo);

	// 应用周期伤害
	ApplyDamageToTargetDataHandle(TargetDataHandle, TickDamageEffect, AbilityLevel);

	// 对每个目标施加减速
	if (SlowEffect)
	{
		const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, 0);
		for (AActor* Target : Targets)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			if (IsValid(TargetASC))
			{
				FGameplayEffectSpecHandle SlowSpec = MakeOutgoingGameplayEffectSpec(SlowEffect, AbilityLevel);
				if (SlowSpec.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*SlowSpec.Data.Get());
				}
			}
		}
	}
}

void UGA_ArcaneStorm::OnStormEnd(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		const int32 AbilityLevel = GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo);

		// 最终爆发伤害
		ApplyDamageToTargetDataHandle(TargetDataHandle, FinalBurstDamageEffect, AbilityLevel);

		// 对区域内所有目标施加沉默
		if (SilenceEffect)
		{
			const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, 0);
			for (AActor* Target : Targets)
			{
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
				if (IsValid(TargetASC))
				{
					FGameplayEffectSpecHandle SilenceSpec = MakeOutgoingGameplayEffectSpec(SilenceEffect, AbilityLevel);
					if (SilenceSpec.IsValid())
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*SilenceSpec.Data.Get());
					}
				}
			}
		}
	}

	// 播放爆发特效 + 相机震动
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = StormLocation;
		CueParams.RawMagnitude = StormRadius;
		OwnerASC->ExecuteGameplayCue(StormExplosionCueTag, CueParams);
		OwnerASC->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), CueParams);
	}

	// 结束技能
	K2_EndAbility();
}

void UGA_ArcaneStorm::AddAimEffect()
{
	if (!AimEffect) return;
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_ArcaneStorm::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}
