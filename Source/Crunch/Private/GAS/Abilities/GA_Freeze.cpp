// 幻雨喜欢小猫咪


#include "GA_Freeze.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TA/TargetActor_GroundPick.h"

UGA_Freeze::UGA_Freeze()
{
	// 添加瞄准状态标签
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Aim);
}

void UGA_Freeze::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 检查网络权限和预测键
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		return;
	}
	
	// 任务1：播放瞄准动画（选择目标位置阶段）
	UAbilityTask_PlayMontageAndWait* PlayTargetingMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, 
		NAME_None, 
		FreezeTargetingMontage // 使用瞄准动画
	);
	// 绑定动画结束事件到能力结束
	PlayTargetingMontage->OnBlendOut.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
	PlayTargetingMontage->OnCancelled.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
	PlayTargetingMontage->OnCompleted.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
	PlayTargetingMontage->OnInterrupted.AddDynamic(this, &UGA_Freeze::K2_EndAbility);
	PlayTargetingMontage->ReadyForActivation(); // 启动任务

	// 任务2：创建目标选择器
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, 
		NAME_None, 
		EGameplayTargetingConfirmation::UserConfirmed, // 需要玩家确认
		TargetActorClass
	);
	// 绑定目标选择完成和取消事件
	WaitTargetData->ValidData.AddDynamic(this, &UGA_Freeze::TargetReceived);
	WaitTargetData->Cancelled.AddDynamic(this, &UGA_Freeze::TargetingCancelled);
	WaitTargetData->ReadyForActivation();

	// 生成目标选择器实例
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor)
	{
		// 配置目标选择器参数
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug()); // 是否显示调试信息
		GroundPickActor->SetTargetAreaRadius(TargetingRadius);  // 作用半径
		GroundPickActor->SetTargetTraceRange(TargetRange);      // 最大距离
	}
	
	// 完成生成
	WaitTargetData->FinishSpawningActor(this, TargetActor);
}

void UGA_Freeze::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 提交技能资源（如魔法值消耗）
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	// 服务端：应用伤害效果
	if (K2_HasAuthority())
	{
		ApplyDamageToTargetDataHandle(TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}

	// 创建冰冻特效参数
	FGameplayCueParameters FreezeCueParameters;
	// 从目标数据中获取命中位置作为特效的生成点
	FreezeCueParameters.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	// 设置特效规模(使用目标半径)
	FreezeCueParameters.RawMagnitude = TargetingRadius;

	// 在目标位置播放冰冻特效（所有客户端可见）
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FreezingGameplayCueTag, FreezeCueParameters);

	// 本地播放施法动画
	PlayMontageLocally(FreezeCastMontage);
	
	// 结束技能
	K2_EndAbility();
}

void UGA_Freeze::TargetingCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 取消目标选择时直接结束技能
	K2_EndAbility();
}
