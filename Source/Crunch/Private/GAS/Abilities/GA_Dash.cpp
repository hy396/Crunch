// 幻雨喜欢小猫咪


#include "GA_Dash.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/TA/TargetActor_Around.h"

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 检查能力是否可提交（资源消耗等）和动画是否有效
	if (!K2_CommitAbility() || !DashMontage)
	{
		// 条件不满足则立即结束能力
		K2_EndAbility();
		return;
	}

	// 确保在服务端或预测有效时执行
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 创建并播放冲刺动画蒙太奇
		UAbilityTask_PlayMontageAndWait* PlayDashMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DashMontage);
		// 绑定动画结束/中断事件到能力结束
		PlayDashMontage->OnBlendOut.AddDynamic(this, &UGA_Dash::K2_EndAbility);
		PlayDashMontage->OnCancelled.AddDynamic(this, &UGA_Dash::K2_EndAbility);
		PlayDashMontage->OnInterrupted.AddDynamic(this, &UGA_Dash::K2_EndAbility);
		PlayDashMontage->OnCompleted.AddDynamic(this, &UGA_Dash::K2_EndAbility);
		PlayDashMontage->ReadyForActivation();

		// 等待动画中的冲刺开始事件
		UAbilityTask_WaitGameplayEvent* WaitDashStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Dash_Start);
		WaitDashStartEvent->EventReceived.AddDynamic(this, &UGA_Dash::StartDash);
		WaitDashStartEvent->ReadyForActivation();
	}
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 获取能力系统组件
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	// 移除冲刺效果
	if (OwnerAbilitySystemComponent && DashEffectHandle.IsValid())
	{
		OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(DashEffectHandle);
	}

	// 清除推进定时器
	if (PushForwardInputTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PushForwardInputTimerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Dash::PushForward()
{
	// 如果存在移动组件，则沿角色前方持续推动
	if (OwnerCharacterMovementComponent)
	{
		// 获取角色前方向量
		FVector ForwardActor = GetAvatarActorFromActorInfo()->GetActorForwardVector();
		// 添加移动输入
		OwnerCharacterMovementComponent->AddInputVector(ForwardActor);
		// 设置下一帧继续推动(循环递归调用)
		PushForwardInputTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGA_Dash::PushForward);
	}
}

void UGA_Dash::StartDash(FGameplayEventData Payload)
{
	// 在服务端应用冲刺效果
	if (K2_HasAuthority())
	{
		if (DashEffect)
		{
			DashEffectHandle = BP_ApplyGameplayEffectToOwner(DashEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}

	// 本地控制角色：启动连续推进
	if (IsLocallyControlled())
	{
		// 启动推进循环
		PushForwardInputTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGA_Dash::PushForward);
	
		// 缓存移动组件
		OwnerCharacterMovementComponent = GetAvatarActorFromActorInfo()->GetComponentByClass<UCharacterMovementComponent>();
	}

	// 创建目标检测任务
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
		this, 
		NAME_None, 
		EGameplayTargetingConfirmation::CustomMulti,  // 自定义确认方式
		TargetActorClass
	);
	
	// 绑定目标检测完成回调
	WaitTargetData->ValidData.AddDynamic(this, &UGA_Dash::TargetReceived);
	WaitTargetData->ReadyForActivation();

	// 生成目标检测器
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);

	// 配置目标检测器
	ATargetActor_Around* TargetActorAround = Cast<ATargetActor_Around>(TargetActor);
	if (TargetActorAround)
	{
		// 设置检测半径、队伍过滤和视觉提示
		TargetActorAround->ConfigureDetection(TargetDetectionRadius, GetOwnerTeamId(), LocalGameplayCueTag);
	}

	// 完成生成
	WaitTargetData->FinishSpawningActor(this, TargetActor);

	// 将检测器附加到角色骨骼
	if (TargetActorAround)
	{
		TargetActorAround->AttachToComponent(
			GetOwningComponentFromActorInfo(), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			TargetActorAttachSocketName
		);
	}
}

void UGA_Dash::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// 服务端处理：对目标应用效果
	if (K2_HasAuthority())
	{
		// 应用伤害效果
		ApplyDamageToTargetDataHandle(TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		// 击退目标
		PushTargetsFromOwnerLocation(TargetDataHandle, TargetHitPushSpeed);
	}
}
