// 幻雨喜欢小猫咪


#include "GA_ArcaneOrb.h"

#include "Actor/ArcaneOrbProjectile.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_ArcaneOrb::UGA_ArcaneOrb()
{
}

void UGA_ArcaneOrb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !CastMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放施法动画
		UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastMontage);
		PlayMontage->OnBlendOut.AddDynamic(this, &UGA_ArcaneOrb::K2_EndAbility);
		PlayMontage->OnCancelled.AddDynamic(this, &UGA_ArcaneOrb::K2_EndAbility);
		PlayMontage->OnInterrupted.AddDynamic(this, &UGA_ArcaneOrb::K2_EndAbility);
		PlayMontage->OnCompleted.AddDynamic(this, &UGA_ArcaneOrb::K2_EndAbility);
		PlayMontage->ReadyForActivation();

		// 等待施法动画事件
		UAbilityTask_WaitGameplayEvent* WaitCastEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_ArcaneOrb_Cast);
		WaitCastEvent->EventReceived.AddDynamic(this, &UGA_ArcaneOrb::OnCast);
		WaitCastEvent->ReadyForActivation();
	}
}

void UGA_ArcaneOrb::OnCast(FGameplayEventData Payload)
{
	if (!K2_HasAuthority())
	{
		return;
	}

	if (!OrbProjectileClass)
	{
		return;
	}

	// 获取发射位置
	FVector SpawnLocation = GetAvatarMeshSocketLocation(CastSocketName);
	if (SpawnLocation.IsZero())
	{
		SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	}

	// 获取发射方向（使用角色朝向）
	FRotator SpawnRotation = GetAvatarActorFromActorInfo()->GetActorRotation();

	// 如果有控制器，使用控制器视角
	if (APlayerController* PC = Cast<APlayerController>(GetAvatarActorFromActorInfo()->GetInstigatorController()))
	{
		FVector ViewLoc;
		FRotator ViewRot;
		PC->GetPlayerViewPoint(ViewLoc, ViewRot);
		SpawnRotation = ViewRot;
	}

	// 生成奥术弹投射物
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	SpawnParams.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AArcaneOrbProjectile* Orb = GetWorld()->SpawnActor<AArcaneOrbProjectile>(
		OrbProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Orb)
	{
		// 设置穿透弹参数
		Orb->SetMaxDistance(OrbMaxDistance);
		Orb->SetDistanceDamageMultiplier(DistanceDamageMultiplier);

		// 设置法力回复效果
		if (ManaRestoreEffect)
		{
			FGameplayEffectSpecHandle ManaSpec = MakeOutgoingGameplayEffectSpec(ManaRestoreEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			Orb->SetManaRestoreEffect(ManaSpec);
		}

		// 创建伤害效果句柄
		FGameplayEffectSpecHandle HitSpec;
		if (DamageEffect.DamageEffect)
		{
			HitSpec = MakeOutgoingGameplayEffectSpec(DamageEffect.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}

		// 发射投射物
		Orb->ShootProjectile(OrbSpeed, OrbMaxDistance, nullptr, GetOwnerTeamId(), HitSpec, DamageEffect);
	}
}
