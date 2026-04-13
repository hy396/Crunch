// 幻雨喜欢小猫咪


#include "GA_MeteorStrike.h"

#include "Actor/BurningGroundActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/TA/TargetActor_Around.h"
#include "GAS/TA/TargetActor_GroundPick.h"

UGA_MeteorStrike::UGA_MeteorStrike()
{
}

void UGA_MeteorStrike::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	// 播放瞄准动画（延迟CommitAbility，参考BlackHole模式）
	TargetingMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	TargetingMontageTask->OnBlendOut.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
	TargetingMontageTask->OnCancelled.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
	TargetingMontageTask->OnInterrupted.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
	TargetingMontageTask->OnCompleted.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
	TargetingMontageTask->ReadyForActivation();

	// 地面目标选择
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &UGA_MeteorStrike::OnTargetConfirmed);
	WaitTargetData->Cancelled.AddDynamic(this, &UGA_MeteorStrike::OnTargetCancelled);
	WaitTargetData->ReadyForActivation();

	// 生成地面选择器
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	if (ATargetActor_GroundPick* GroundPick = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		GroundPick->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPick->SetTargetAreaRadius(TargetAreaRadius);
		GroundPick->SetTargetTraceRange(CastRange);
	}
	WaitTargetData->FinishSpawningActor(this, TargetActor);

	AddAimEffect();
}

void UGA_MeteorStrike::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAimEffect();

	if (MeteorFallTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MeteorFallTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeteorStrike::OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	RemoveAimEffect();

	// 解除瞄准动画的结束绑定
	if (TargetingMontageTask)
	{
		TargetingMontageTask->OnBlendOut.RemoveAll(this);
		TargetingMontageTask->OnCancelled.RemoveAll(this);
		TargetingMontageTask->OnInterrupted.RemoveAll(this);
		TargetingMontageTask->OnCompleted.RemoveAll(this);
	}

	// 播放施法动画
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayCastMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastMontage);
		PlayCastMontage->OnBlendOut.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
		PlayCastMontage->OnCancelled.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
		PlayCastMontage->OnInterrupted.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
		PlayCastMontage->OnCompleted.AddDynamic(this, &UGA_MeteorStrike::K2_EndAbility);
		PlayCastMontage->ReadyForActivation();
	}

	// 缓存落点位置
	CachedImpactLocation = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0).ImpactPoint;

	// 延迟后陨石落地
	GetWorld()->GetTimerManager().SetTimer(MeteorFallTimerHandle, this, &UGA_MeteorStrike::OnMeteorImpact, MeteorFallDelay);
}

void UGA_MeteorStrike::OnTargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_MeteorStrike::OnMeteorImpact()
{
	if (!K2_HasAuthority() || !ImpactTargetActorClass)
	{
		return;
	}

	// 在落点生成圆形范围检测
	UAbilityTask_WaitTargetData* WaitImpactData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, ImpactTargetActorClass);
	WaitImpactData->ValidData.AddDynamic(this, &UGA_MeteorStrike::OnImpactTargetReceived);
	WaitImpactData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitImpactData->BeginSpawningActor(this, ImpactTargetActorClass, TargetActor);
	if (ATargetActor_Around* AroundActor = Cast<ATargetActor_Around>(TargetActor))
	{
		AroundActor->ConfigureDetection(TargetAreaRadius, GetOwnerTeamId(), MeteorImpactCueTag);
	}
	WaitImpactData->FinishSpawningActor(this, TargetActor);

	// 将检测器放到落点位置
	if (TargetActor)
	{
		TargetActor->SetActorLocation(CachedImpactLocation);
	}

	// 播放陨石落地特效 + 相机震动
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = CachedImpactLocation;
		CueParams.RawMagnitude = TargetAreaRadius;
		OwnerASC->ExecuteGameplayCue(MeteorImpactCueTag, CueParams);
		OwnerASC->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), CueParams);
	}

	// 生成燃烧地面
	if (BurningGroundActorClass && BurnDamageEffect)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetAvatarActorFromActorInfo();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABurningGroundActor* BurnZone = GetWorld()->SpawnActor<ABurningGroundActor>(
			BurningGroundActorClass, CachedImpactLocation, FRotator::ZeroRotator, SpawnParams);

		if (BurnZone)
		{
			FGameplayEffectSpecHandle BurnSpec = MakeOutgoingGameplayEffectSpec(
				BurnDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			BurnZone->InitializeBurningGround(GetOwnerTeamId(), BurnSpec, TargetAreaRadius, BurnZoneDuration, BurnTickInterval);
		}
	}
}

void UGA_MeteorStrike::OnImpactTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		ApplyDamageToTargetDataHandle(TargetDataHandle, ImpactDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		PushTargetsFromLocation(TargetDataHandle, CachedImpactLocation, ImpactPushSpeed);
	}
}

void UGA_MeteorStrike::AddAimEffect()
{
	if (!AimEffect) return;
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_MeteorStrike::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}
