// 幻雨喜欢小猫咪


#include "GA_BlackHole.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TA/TargetActor_GroundPick.h"
#include "GAS/TA/TargetActor_BlackHole.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/Core/CAbilitySystemStatics.h"

void UGA_BlackHole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}
	// 播放起手动画，动画结束/中断/取消时自动结束技能
	PlayCastBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayCastBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->ReadyForActivation();

	// 等待瞄准敌人
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	// 确认技能
	WaitTargetDataTask->ValidData.AddDynamic(this, &UGA_BlackHole::PlaceBlackHole);
	// 技能取消
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UGA_BlackHole::PlacementCancelled);
	WaitTargetDataTask->ReadyForActivation();
	// 在UAbilityTask_WaitTargetData任务的确认或者取消的时候会自动清除TargetActor
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
	
	// 添加瞄准GE
	AddAimEffect();
}

void UGA_BlackHole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 移除瞄准GE
	RemoveAimEffect();
	// 移除焦点GE
	// RemoveFocusEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BlackHole::PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 生成黑洞
	// 能力消耗与条件校验
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	// 移除瞄准tag
	RemoveAimEffect();
	// AddFocusEffect();
	// 移除动画任务绑定的委托
	if (PlayCastBlackHoleMontageTask)
	{
		PlayCastBlackHoleMontageTask->OnBlendOut.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnInterrupted.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCompleted.RemoveAll(this);
	}
	
	// 权限校验，播放蓄力动画
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHoldBlackHoleMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoldBlackHoleMontage);
		// PlayHoldBlackHoleMontage->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		// PlayHoldBlackHoleMontage->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		// PlayHoldBlackHoleMontage->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		// PlayHoldBlackHoleMontage->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayHoldBlackHoleMontage->ReadyForActivation();
	}

	// 创建等待黑洞数据传输任务
	// BlackHoleTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(
	// 	this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackHoleTargetActorClass);

	BlackHoleTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, // 持续检测模式
		BlackHoleTargetActorClass);


	// 黑洞爆炸任务完成
	// BlackHoleTargetingTask->ValidData.AddDynamic(this, &UGA_BlackHole::FinalTargetsReceived);
	// 抄袭激光技能
	BlackHoleTargetingTask->ValidData.AddDynamic(this, &UGA_BlackHole::TargetReceived);
	// 黑洞爆炸任务完成
	BlackHoleTargetingTask->Cancelled.AddDynamic(this, &UGA_BlackHole::FinalTargetsReceived);
	BlackHoleTargetingTask->ReadyForActivation();

	// 生成黑洞目标Actor
	AGameplayAbilityTargetActor* TargetActor;
	BlackHoleTargetingTask->BeginSpawningActor(this, BlackHoleTargetActorClass, TargetActor);
	ATargetActor_BlackHole* BlackHoleTargetActor = Cast<ATargetActor_BlackHole>(TargetActor);
	if (BlackHoleTargetActor)
	{
		// 配置黑洞参数
		BlackHoleTargetActor->ConfigureBlackHole(TargetAreaRadius, BlackHolePullSpeed, TargetingInterval, BlackHoleDuration, GetOwnerTeamId());
	}
	BlackHoleTargetingTask->FinishSpawningActor(this, TargetActor);
	if (BlackHoleTargetActor)
	{
		// 配置黑洞位置
		BlackHoleTargetActor->SetActorLocation(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint);
		UE_LOG(LogTemp, Warning, TEXT("GA中设置的 Location: %s"), *BlackHoleTargetActor->GetActorLocation().ToString())
	}
}

void UGA_BlackHole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 取消则结束技能
	K2_EndAbility();
}

void UGA_BlackHole::FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		// 对目标应用最终伤害效果
		ApplyDamageToTargetDataHandle(TargetDataHandle, FinalBlowDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		// BP_ApplyGameplayEffectToTarget(TargetDataHandle, FinalBlowDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		FVector BlowCenter = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
		// 推开所有目标
		PushTargetsFromLocation(TargetDataHandle, BlowCenter, BlowPushSpeed);

		// 播放终结动画
		UAbilityTask_PlayMontageAndWait* PlayFinalBlowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlowMontage);
		PlayFinalBlowMontage->OnBlendOut.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->OnCancelled.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->OnInterrupted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->OnCompleted.AddDynamic(this, &UGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontage->ReadyForActivation();
	}
	else
	{
		// 客户端本地播放终结动画
		PlayMontageLocally(FinalBlowMontage);
	}

	// 播放终结特效和相机震动
	FGameplayCueParameters FinalBlowCueParams;
	FinalBlowCueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	FinalBlowCueParams.RawMagnitude = TargetAreaRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FinalBlowCueTag, FinalBlowCueParams);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), FinalBlowCueParams);
}

void UGA_BlackHole::AddAimEffect()
{
	// 应用瞄准GE
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffect);
}

void UGA_BlackHole::RemoveAimEffect()
{
	// 移除瞄准GE
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}

void UGA_BlackHole::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		ApplyDamageToTargetDataHandle(TargetDataHandle, BlackHoleHitEffectDef, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
}

// void UGA_BlackHole::AddFocusEffect()
// {
// 	FocusEffectHandle = BP_ApplyGameplayEffectToOwner(FocusEffect);
// }
//
// void UGA_BlackHole::RemoveFocusEffect()
// {
// 	if (FocusEffectHandle.IsValid())
// 	{
// 		BP_RemoveGameplayEffectFromOwnerWithHandle(FocusEffectHandle);
// 	}
// }
