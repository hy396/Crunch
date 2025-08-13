// 幻雨喜欢小猫咪


#include "GA_Laser.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/TA/TargetActor_Line.h"

void UGA_Laser::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !LaserMontage)
	{
		K2_EndAbility();
		return;
	}

	// 仅在服务器或预测有效时执行
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放激光蒙太奇
		UAbilityTask_PlayMontageAndWait* PlayerLaserMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LaserMontage);
		PlayerLaserMontageTask->OnBlendOut.AddDynamic(this, &UGA_Laser::K2_EndAbility);
		PlayerLaserMontageTask->OnCancelled.AddDynamic(this, &UGA_Laser::K2_EndAbility);
		PlayerLaserMontageTask->OnInterrupted.AddDynamic(this, &UGA_Laser::K2_EndAbility);
		PlayerLaserMontageTask->OnCompleted.AddDynamic(this, &UGA_Laser::K2_EndAbility);
		PlayerLaserMontageTask->ReadyForActivation();

		// 等待动画事件触发激光发射
		UAbilityTask_WaitGameplayEvent* WaitShootEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Laser_Shoot);
		WaitShootEvent->EventReceived.AddDynamic(this, &UGA_Laser::ShootLaser);
		WaitShootEvent->ReadyForActivation();
		
		// 设置技能取消监听
		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &UGA_Laser::K2_EndAbility);
		WaitCancel->ReadyForActivation();
	}
}

void UGA_Laser::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 移除持续消耗法力的效果
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (OwnerAbilitySystemComponent && OnGoingConsumptionEffectHandle.IsValid())
	{
		OwnerAbilitySystemComponent->RemoveActiveGameplayEffect(OnGoingConsumptionEffectHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Laser::ShootLaser(FGameplayEventData Payload)
{
	// --- 服务器端逻辑 ---
	if (K2_HasAuthority())
	{
		// 应用持续消耗法力的效果
		OnGoingConsumptionEffectHandle = BP_ApplyGameplayEffectToOwner(OnGoingConsumptionEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		
		// 注册法力变化回调（检测法力不足）
		UAbilitySystemComponent* OwnerAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
		if (OwnerAbilitySystemComponent)
		{
			OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &UGA_Laser::ManaUpdated);
		}
	}

	// 创建目标检测任务
	UAbilityTask_WaitTargetData* WaitDamageTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(
		this, 
		NAME_None, 
		EGameplayTargetingConfirmation::CustomMulti, // 持续检测模式
		LaserTargetActorClass
	);
	// 绑定委托
	WaitDamageTargetTask->ValidData.AddDynamic(this, &UGA_Laser::TargetReceived);
	// 执行任务
	WaitDamageTargetTask->ReadyForActivation();

	// 配置并生成目标检测Actor
	AGameplayAbilityTargetActor* TargetActor;
	WaitDamageTargetTask->BeginSpawningActor(this, LaserTargetActorClass, TargetActor);
	if (ATargetActor_Line* LineTargetActor = Cast<ATargetActor_Line>(TargetActor))
	{
		// 设置检测参数：范围/半径/频率/队伍/调试显示
		LineTargetActor->ConfigureTargetSetting(
			TargetRange, 
			DetectionCylinderRadius, 
			TargetingInterval, 
			GetOwnerTeamId(), 
			ShouldDrawDebug()
		);
	}
	WaitDamageTargetTask->FinishSpawningActor(this, TargetActor);

	// 将Actor附加到骨骼中
	if (ATargetActor_Line* LineTargetActor = Cast<ATargetActor_Line>(TargetActor))
	{
		LineTargetActor->AttachToComponent(
			GetOwningComponentFromActorInfo(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TargetActorAttachSocketName
		);
	}
}

void UGA_Laser::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	// 当法力不足时自动结束技能
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo() ;
	if (OwnerASC && !OwnerASC->CanApplyAttributeModifiers(
		OnGoingConsumptionEffect.GetDefaultObject(),
		GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo), 
		MakeEffectContext(CurrentSpecHandle, CurrentActorInfo)))
	{
		K2_EndAbility();
	}
}

void UGA_Laser::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// --- 服务器端逻辑 ---
	if (K2_HasAuthority())
	{
		// 1. 对目标应用伤害效果
		// BP_ApplyGameplayEffectToTarget(
		// 	TargetDataHandle, 
		// 	HitDamageEffect, 
		// 	GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)
		// );
		// K2_ApplyGameplayEffectSpecToTarget(,TargetDataHandle);
		ApplyDamageToTargetDataHandle(TargetDataHandle, HitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
	
	// 2. 对目标施加击退力
	PushTargets(
		TargetDataHandle, 
		GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitDamageEffect.PushVelocity // 基于发射者方向的击退
	);
}
