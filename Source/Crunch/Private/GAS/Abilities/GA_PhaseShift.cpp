// 幻雨喜欢小猫咪


#include "GA_PhaseShift.h"

#include "Actor/ArcaneTrapActor.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Core/CAbilitySystemStatics.h"

UGA_PhaseShift::UGA_PhaseShift()
{
	// 相位转移期间无敌 + 相位穿透
	ActivationOwnedTags.AddTag(TGameplayTags::Status::Invincible);
	ActivationOwnedTags.AddTag(TGameplayTags::Status::Phasing);
}

void UGA_PhaseShift::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !PhaseShiftMontage)
	{
		K2_EndAbility();
		return;
	}

	// 缓存当前位置（用于放置陷阱）
	CachedOriginLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放相位转移动画
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PhaseShiftMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UGA_PhaseShift::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UGA_PhaseShift::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UGA_PhaseShift::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UGA_PhaseShift::K2_EndAbility);
		PlayMontage->ReadyForActivation();

		// 等待位移动画事件
		UAbilityTask_WaitGameplayEvent* WaitShiftEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_PhaseShift_Shift);
		WaitShiftEvent->EventReceived.AddDynamic(this, &UGA_PhaseShift::OnShift);
		WaitShiftEvent->ReadyForActivation();
	}

	// 播放特效
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = CachedOriginLocation;
		OwnerASC->ExecuteGameplayCue(PhaseShiftCueTag, CueParams);
	}
}

void UGA_PhaseShift::OnShift(FGameplayEventData Payload)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	// 向后位移
	FVector BackwardDir = -AvatarActor->GetActorForwardVector();
	PushSelf(BackwardDir * BackwardPushSpeed);

	// 服务器端放置陷阱
	if (K2_HasAuthority() && TrapActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = AvatarActor;
		SpawnParams.Instigator = Cast<APawn>(AvatarActor);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AArcaneTrapActor* Trap = GetWorld()->SpawnActor<AArcaneTrapActor>(
			TrapActorClass, CachedOriginLocation, FRotator::ZeroRotator, SpawnParams);

		if (Trap)
		{
			int32 AbilityLevel = GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo);

			// 创建伤害效果句柄
			FGameplayEffectSpecHandle DamageSpec;
			if (TrapDamageEffect)
			{
				DamageSpec = MakeOutgoingGameplayEffectSpec(TrapDamageEffect, AbilityLevel);
			}

			// 创建定身效果句柄
			FGameplayEffectSpecHandle RootSpec;
			if (RootEffect)
			{
				RootSpec = MakeOutgoingGameplayEffectSpec(RootEffect, AbilityLevel);
			}

			Trap->InitializeTrap(GetOwnerTeamId(), DamageSpec, RootSpec, TrapRadius, TrapArmDelay, TrapDuration);
		}
	}
}
