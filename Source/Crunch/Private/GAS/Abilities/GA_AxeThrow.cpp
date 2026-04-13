// 幻雨喜欢小猫咪


#include "GA_AxeThrow.h"

#include "Actor/BoomerangAxeProjectile.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_AxeThrow::UGA_AxeThrow()
{
}

void UGA_AxeThrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !ThrowMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放投掷动画
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ThrowMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UGA_AxeThrow::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UGA_AxeThrow::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UGA_AxeThrow::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UGA_AxeThrow::K2_EndAbility);
		PlayMontage->ReadyForActivation();

		// 等待投掷释放动画事件
		UAbilityTask_WaitGameplayEvent* WaitRelease = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_AxeThrow_Release);
		WaitRelease->EventReceived.AddDynamic(this, &UGA_AxeThrow::OnRelease);
		WaitRelease->ReadyForActivation();

		// 等待斧头返回事件
		UAbilityTask_WaitGameplayEvent* WaitReturn = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_AxeThrow_Return);
		WaitReturn->EventReceived.AddDynamic(this, &UGA_AxeThrow::OnAxeReturned);
		WaitReturn->ReadyForActivation();
	}
}

void UGA_AxeThrow::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AxeThrow::OnRelease(FGameplayEventData Payload)
{
	if (!K2_HasAuthority() || !AxeProjectileClass)
	{
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	// 获取发射位置和方向
	FVector SpawnLocation = GetAvatarMeshSocketLocation(ThrowSocketName);
	if (SpawnLocation.IsZero())
	{
		SpawnLocation = AvatarActor->GetActorLocation();
	}

	FRotator SpawnRotation = AvatarActor->GetActorRotation();
	if (APlayerController* PC = Cast<APlayerController>(AvatarActor->GetInstigatorController()))
	{
		FVector ViewLoc;
		FRotator ViewRot;
		PC->GetPlayerViewPoint(ViewLoc, ViewRot);
		SpawnRotation = ViewRot;
	}

	// 生成回旋斧
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(AvatarActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABoomerangAxeProjectile* Axe = GetWorld()->SpawnActor<ABoomerangAxeProjectile>(
		AxeProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Axe)
	{
		// 设置返回事件Tag
		Axe->SetReturnEventTag(TGameplayTags::Ability_AxeThrow_Return);

		// 创建伤害效果句柄
		FGameplayEffectSpecHandle HitSpec;
		if (DamageEffect.DamageEffect)
		{
			HitSpec = MakeOutgoingGameplayEffectSpec(DamageEffect.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}

		// 发射
		Axe->ShootProjectile(ThrowSpeed, MaxThrowDistance, nullptr, GetOwnerTeamId(), HitSpec, DamageEffect);
	}
}

void UGA_AxeThrow::OnAxeReturned(FGameplayEventData Payload)
{
	// 播放接住动画
	if (CatchMontage)
	{
		PlayMontageLocally(CatchMontage);
	}

	// 斧头回来了，结束技能
	K2_EndAbility();
}
