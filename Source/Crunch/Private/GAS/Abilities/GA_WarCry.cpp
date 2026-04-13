// 幻雨喜欢小猫咪


#include "GA_WarCry.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/TA/TargetActor_Around.h"

UGA_WarCry::UGA_WarCry()
{
}

void UGA_WarCry::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !WarCryMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放战吼动画
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, WarCryMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UGA_WarCry::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UGA_WarCry::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UGA_WarCry::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UGA_WarCry::K2_EndAbility);
		PlayMontage->ReadyForActivation();

		// 等待动画中的吼叫事件
		UAbilityTask_WaitGameplayEvent* WaitShoutEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_WarCry_Shout);
		WaitShoutEvent->EventReceived.AddDynamic(this, &UGA_WarCry::OnShout);
		WaitShoutEvent->ReadyForActivation();
	}
}

void UGA_WarCry::OnShout(FGameplayEventData Payload)
{
	// 创建圆形范围检测
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this,
		NAME_None,
		EGameplayTargetingConfirmation::CustomMulti,
		TargetActorClass
	);

	WaitTargetData->ValidData.AddDynamic(this, &UGA_WarCry::TargetReceived);
	WaitTargetData->ReadyForActivation();

	// 生成检测器
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);

	if (ATargetActor_Around* AroundActor = Cast<ATargetActor_Around>(TargetActor))
	{
		AroundActor->ConfigureDetection(DetectionRadius, GetOwnerTeamId(), WarCryCueTag);
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

	// 播放战吼特效 + 相机震动
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
		CueParams.RawMagnitude = DetectionRadius;
		OwnerASC->ExecuteGameplayCue(WarCryCueTag, CueParams);
		OwnerASC->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), CueParams);
	}
}

void UGA_WarCry::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		// 应用物理伤害
		ApplyDamageToTargetDataHandle(TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

		// 轻微击退
		PushTargetsFromOwnerLocation(TargetDataHandle, PushSpeed);

		// 对每个目标施加减甲减速Debuff
		if (DebuffEffect)
		{
			const TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetDataHandle, 0);
			for (AActor* Target : Targets)
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
				{
					FGameplayEffectSpecHandle DebuffSpec = MakeOutgoingGameplayEffectSpec(DebuffEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
					if (DebuffSpec.IsValid())
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*DebuffSpec.Data.Get());
					}
				}
			}
		}
	}
}
