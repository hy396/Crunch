// 幻雨喜欢小猫咪


#include "GA_Tornado.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

void UGA_Tornado::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 提交技能资源（如冷却、消耗等），失败则结束
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 任务1：播放龙卷风动画
		UAbilityTask_PlayMontageAndWait* PlayTornadoMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, 
			NAME_None, 
			TornadoMontage
		);
		// 绑定动画结束/中断事件
		PlayTornadoMontage->OnBlendOut.AddDynamic(this, &UGA_Tornado::K2_EndAbility);
		PlayTornadoMontage->OnCancelled.AddDynamic(this, &UGA_Tornado::K2_EndAbility);
		PlayTornadoMontage->OnInterrupted.AddDynamic(this, &UGA_Tornado::K2_EndAbility);
		PlayTornadoMontage->OnCompleted.AddDynamic(this, &UGA_Tornado::K2_EndAbility);
		PlayTornadoMontage->ReadyForActivation(); // 启动任务

		// 任务2：在服务端监听伤害事件
		if (K2_HasAuthority())
		{
			// 等待监听伤害事件
			UAbilityTask_WaitGameplayEvent* WaitDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this, 
				TGameplayTags::Ability_Generic_Damage,
				nullptr, 
				false
			);
			// 绑定事件回调
			WaitDamageEvent->EventReceived.AddDynamic(this, &UGA_Tornado::TornadoDamageEventReceived);
			WaitDamageEvent->ReadyForActivation();
		}

		// 任务3：等待技能取消（如玩家提前中断）
		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &UGA_Tornado::K2_EndAbility);
		WaitCancel->ReadyForActivation();

		// 任务4：设置技能超时（基于持续时间）
		UAbilityTask_WaitDelay* WaitTornadoTimeout = UAbilityTask_WaitDelay::WaitDelay(this, TornadoDuration);
		WaitTornadoTimeout->OnFinish.AddDynamic(this, &UGA_Tornado::K2_EndAbility);
		WaitTornadoTimeout->ReadyForActivation();
	}
}

void UGA_Tornado::TornadoDamageEventReceived(FGameplayEventData Payload)
{
	// 服务器中执行
	if (K2_HasAuthority())
	{
		// 获取目标数据
		FGameplayAbilityTargetDataHandle TargetDataHandle = Payload.TargetData;
		// 应用伤害
		ApplyDamageToTargetDataHandle(TargetDataHandle, HitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

		// 击退目标
		PushTargetsFromOwnerLocation(TargetDataHandle, HitPushSpeed);
	}
}
