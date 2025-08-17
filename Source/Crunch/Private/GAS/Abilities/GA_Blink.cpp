// 幻雨喜欢小猫咪


#include "GA_Blink.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TA/TargetActor_GroundPick.h"
// #include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"

UGA_Blink::UGA_Blink()
{
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Aim);
}

void UGA_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 检查网络权限和预测键
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	// 任务1：播放瞄准动画
	UAbilityTask_PlayMontageAndWait* PlayTargetingMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, 
		NAME_None, 
		TargetingMontage
	);
	PlayTargetingMontage->ReadyForActivation();

	// 任务2：创建目标选择器
	UAbilityTask_WaitTargetData* WaitBlinkLocationTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, 
		NAME_None, 
		EGameplayTargetingConfirmation::UserConfirmed, // 需要玩家确认
		GroundPickTargetActorClass
	);
	// 绑定目标选择完成和取消事件
	WaitBlinkLocationTargetData->ValidData.AddDynamic(this, &UGA_Blink::GroundPickTargetReceived);
	WaitBlinkLocationTargetData->Cancelled.AddDynamic(this, &UGA_Blink::GroundPickCancelled);
	WaitBlinkLocationTargetData->ReadyForActivation();

	// 生成目标选择器实例
	AGameplayAbilityTargetActor* TargetActor;
	WaitBlinkLocationTargetData->BeginSpawningActor(this, GroundPickTargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickTargetActor)
	{
		// 配置目标选择器参数
		GroundPickTargetActor->SetShouldDrawDebug(ShouldDrawDebug()); // 是否显示调试信息
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius); // 作用半径
		GroundPickTargetActor->SetTargetTraceRange(BlinkCastRange);   // 最大距离
	}

	// 完成生成
	WaitBlinkLocationTargetData->FinishSpawningActor(this, TargetActor);
}

void UGA_Blink::GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 提交技能资源（如魔法值消耗）
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	// 缓存目标位置数据(传送完成后造成伤害用)
	BlinkTargetDataHandle = TargetDataHandle;

	// 确保在服务端或预测有效时执行
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		// 任务3：播放传送动画
		UAbilityTask_PlayMontageAndWait* PlayTeleportMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, 
			NAME_None, 
			TeleportMontage
		);
		// 绑定动画结束事件到能力结束
		PlayTeleportMontage->OnBlendOut.AddDynamic(this, &UGA_Blink::K2_EndAbility);
		PlayTeleportMontage->OnCancelled.AddDynamic(this, &UGA_Blink::K2_EndAbility);
		PlayTeleportMontage->OnInterrupted.AddDynamic(this, &UGA_Blink::K2_EndAbility);
		PlayTeleportMontage->OnCompleted.AddDynamic(this, &UGA_Blink::K2_EndAbility);
		PlayTeleportMontage->ReadyForActivation();
		
		// 任务4：在服务端监听传送事件（动画通知触发）
		if (K2_HasAuthority())
		{
			UAbilityTask_WaitGameplayEvent* WaitTeleportTimePoint = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this, 
				TGameplayTags::Ability_Blink_Teleport
			);
			UE_LOG(LogTemp, Warning, TEXT("绑定动画回调"))
			// 绑定传送事件回调
			WaitTeleportTimePoint->EventReceived.AddDynamic(this, &UGA_Blink::Teleport);
			WaitTeleportTimePoint->ReadyForActivation();
		}
	}
}

void UGA_Blink::GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_Blink::Teleport(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		// 从目标数据获取选择的传送位置
		FHitResult PickedLocationHitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(BlinkTargetDataHandle, 1);
		FVector PickedTeleportLocation = PickedLocationHitResult.ImpactPoint;

		UE_LOG(LogTemp, Warning, TEXT("当前位置1:%s"),*GetAvatarActorFromActorInfo()->GetActorLocation().ToString())
		// 将角色传送到目标位置
		GetAvatarActorFromActorInfo()->SetActorLocation(PickedTeleportLocation);

		UE_LOG(LogTemp, Warning, TEXT("当前位置2:%s"),*GetAvatarActorFromActorInfo()->GetActorLocation().ToString())
		// 对落点范围内的目标应用伤害效果
		ApplyDamageToTargetDataHandle(
			BlinkTargetDataHandle, 
			DamageEffect, 
			GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
			);
		
		// 击退目标
		PushTargetsFromOwnerLocation(BlinkTargetDataHandle, BlinkLandTargetPushSpeed);
	}
}
