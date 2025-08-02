// 幻雨喜欢小猫咪


#include "GAS/Abilities/GA_GroundBlast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/Core/TGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/Core/CAbilitySystemStatics.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	// 技能激活时给角色添加瞄准标签
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Aim);
	// 阻断基础攻击技能
	BlockAbilitiesWithTag.AddTag(TGameplayTags::Ability_BasicAttack);
}

void UGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) return;

	UAbilityTask_PlayMontageAndWait* PlayGroundBlasAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayGroundBlasAnimTask->OnBlendOut.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlasAnimTask->OnCancelled.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlasAnimTask->OnCompleted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlasAnimTask->OnInterrupted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlasAnimTask->ReadyForActivation();

	// 等待瞄准敌人
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	// 确认技能
	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_GroundBlast::TargetConfirmed);
	// 技能取消
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_GroundBlast::TargetCanceled);
	WaitTargetDataTask->ReadyForActivation();

	// 生成目标Actor
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);

	// 设置目标Actor参数
	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor)
	{
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickActor->SetTargetTraceRange(TargetTraceRange);
	}
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
}

void UGA_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	// 仅在服务器上执行伤害和击退
	if (K2_HasAuthority())
	{
		// MakeDamage(DamageEffectDef,GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		// 对目标应用伤害效果
		// BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffectDef.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

		// 获取命中目标的数量
		TArray<AActor*> HitActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
		for (int32 i = 0; i < HitActors.Num(); ++i)
		{
			AActor* HitActor = HitActors[i];
			// 检查 HitResult 是否有效
			if (HitActor)
			{
				// UE_LOG(LogTemp, Warning, TEXT("命中Actor: %s"), *HitActor->GetName());
				ApplyDamage(HitActor, DamageEffectDef, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			}
		}
		// ApplyDamage
		// 对目标施加推力
		PushTargets(TargetDataHandle, DamageEffectDef.PushVelocity);
	}

	FGameplayCueParameters BlastingGameplayCueParameters;
	// 设置特效的位置
	BlastingGameplayCueParameters.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	// 设置特效的大小
	BlastingGameplayCueParameters.RawMagnitude = TargetAreaRadius;

	// 播放冲击特效和摄像机震动
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(BlastGameplayCueTag, BlastingGameplayCueParameters);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), BlastingGameplayCueParameters);

	// 播放释放动画
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst)
	{
		OwnerAnimInst->Montage_Play(CastMontage);
	}
	UE_LOG(LogTemp, Warning, TEXT("技能发射"));
	K2_EndAbility();
	// 连发操作
	//  --NumCount;
	// if (NumCount > 0)
	// {
	// 	// 等待瞄准敌人
	// 	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	// 	// 确认技能
	// 	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_GroundBlast::TargetConfirmed);
	// 	// 技能取消
	// 	WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_GroundBlast::TargetCanceled);
	// 	WaitTargetDataTask->ReadyForActivation();
	//
	// 	// 生成目标Actor
	// 	AGameplayAbilityTargetActor* TargetActor;
	// 	WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	//
	// 	// 设置目标Actor参数
	// 	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	// 	if (GroundPickActor)
	// 	{
	// 		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
	// 		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
	// 		GroundPickActor->SetTargetTraceRange(TargetTraceRange);
	// 	}
	// 	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
	// }else
	// {
	// 	K2_EndAbility();
	// }
}

void UGA_GroundBlast::TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("技能取消"));
	K2_EndAbility();
}
