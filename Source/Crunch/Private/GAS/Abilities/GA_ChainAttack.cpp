// 幻雨喜欢小猫咪


#include "GA_ChainAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Character/Interaction/CombatInterface.h"
// #include "Engine/OverlapResult.h"
#include "GAS/TA/TargetActor_Around.h"
// #include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
// #include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
// #include "GAS/Core/CAbilitySystemStatics.h"


UGA_ChainAttack::UGA_ChainAttack()
{
}

void UGA_ChainAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	CurrentAttackIndex = 0;
	// 检查能力是否可提交（资源消耗等）和动画是否有效
	if (!K2_CommitAbility())
	{
		// 条件不满足则立即结束能力
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 播放蓄力动画
		// 不想这个动画会会导致后续不能进行
		UAbilityTask_PlayMontageAndWait* PlayChargeMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ChargeMontage);
		// 绑定动画结束/中断事件到能力结束
		PlayChargeMontage->OnBlendOut.AddDynamic(this, &UGA_ChainAttack::K2_EndAbility);
		PlayChargeMontage->OnCancelled.AddDynamic(this, &UGA_ChainAttack::K2_EndAbility);
		PlayChargeMontage->OnInterrupted.AddDynamic(this, &UGA_ChainAttack::K2_EndAbility);
		PlayChargeMontage->OnCompleted.AddDynamic(this, &UGA_ChainAttack::K2_EndAbility);
		PlayChargeMontage->ReadyForActivation();	
		// 等待动画中的冲刺开始事件
		UAbilityTask_WaitGameplayEvent* WaitDashStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_ChainAttack);
		WaitDashStartEvent->EventReceived.AddDynamic(this, &UGA_ChainAttack::StartAttackTimer);
		WaitDashStartEvent->ReadyForActivation();
	}
}

void UGA_ChainAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// // 停止攻击定时器
	// StopAttacks();
	// // 执行最后一击范围伤害
	// ExecuteLastHit();
	// if (MoveTask)
	// {
	// 	MoveTask->EndTask();
	// 	MoveTask = nullptr;
	// }
	StopAttacks();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ChainAttack::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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

FVector UGA_ChainAttack::CalculateCenter()
{
	if (TargetActors.Num() == 0)
	{
		if (const AActor* Owner = GetAvatarActorFromActorInfo())
		{
			return Owner->GetActorLocation();
		}
		return FVector::ZeroVector;
	}
	
	FVector NewCenter = FVector::ZeroVector;
	int32 ValidCount = 0;

	for (const TWeakObjectPtr<AActor>& Actor : TargetActors)
	{
		if (Actor.IsValid())
		{
			NewCenter += Actor->GetActorLocation();
			ValidCount++;
		}
	}

	if (ValidCount > 0)
	{
		NewCenter /= ValidCount;
	}

	return NewCenter;
}

void UGA_ChainAttack::DetectTargets()
{
	// 清空之前的目标列表
	TargetActors.Empty();

	// 获取角色位置
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	FVector Origin = AvatarActor->GetActorLocation();
	// 获取角色的队伍接口
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(AvatarActor);
	// 设置检测对象类型为Pawn
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 只检测Pawn类型
	TArray<FHitResult> HitResults; // 用于存储命中结果
	TArray<AActor*> IgnoredActors; // 忽略的Actor
	IgnoredActors.Add(AvatarActor);// 忽略自身
	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::ForDuration;// : EDrawDebugTrace::None;
	
	// 球形多重检测，查找路径上的所有目标
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		AvatarActor, Origin, Origin, TargetRange, 
		ObjectTypes, false, IgnoredActors, DrawDebugTrace, HitResults, false);

	// 遍历所有命中结果
	for (const FHitResult& Overlap : HitResults)
	{
		// 忽略已命中的Actor
		AActor* HitActor = Overlap.GetActor();
		// 不存在命中的Actor则跳过
		if (!HitActor)
		{
			continue;
		}
		// 检查目标阵营关系
		if (OwnerTeamInterface)
		{
			if (OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor) != ETeamAttitude::Hostile)
			{
				continue;
			}
		}	
		TargetActors.Add(HitActor);
	}
	// 计算中心点，缓存
	Center = CalculateCenter();
}

void UGA_ChainAttack::StartAttackTimer(FGameplayEventData Payload)
{
	CurrentAttackIndex = 0;
	// 检测范围内的敌人
	DetectTargets();

	// 如果有目标，开始攻击定时器
	if (TargetActors.Num() > 0)
	{
		// 创建目标Actor
		if (TargetActorClass)
		{
			// 创建目标检测任务
			UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(
				this, 
				NAME_None, 
				EGameplayTargetingConfirmation::CustomMulti,  // 自定义确认方式
				TargetActorClass
			);
			// 绑定目标检测完成回调
			WaitTargetData->ValidData.AddDynamic(this, &UGA_ChainAttack::TargetReceived);
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
		}else
		{
			K2_EndAbility();
		}
	}
	else
	{
		// 没有目标，直接结束技能
		K2_EndAbility();
	}
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (!OwnerAnimInst) return;
	// 设置蒙太奇自动切换到下一个片段，达成连击的效果
	OwnerAnimInst->Montage_SetNextSection(OwnerAnimInst->Montage_GetCurrentSection(ChargeMontage), DashName, ChargeMontage);
	// 设置定时器，间隔执行攻击
	GetWorld()->GetTimerManager().SetTimer(
		ChainAttackTimerHandle,
		this,
		&UGA_ChainAttack::ExecuteSingleAttack,
		AttackInterval,
		true,  // 循环执行
		0.0f   // 立即开始
	);
}

void UGA_ChainAttack::ExecuteSingleAttack()
{
	// 检查是否还有目标需要攻击
	if (CurrentAttackIndex >= TargetActors.Num())
	{
		// 攻击完成，结束技能
		// K2_EndAbility();
		// 调用最后一击，停止攻击定时器
		ExecuteLastHit();
		return;
	}
	// 获取当前要攻击的目标
	TWeakObjectPtr<AActor> Target = TargetActors[CurrentAttackIndex];
	if (Target.IsValid())
	{
		// 更新目标位置
		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			if (Avatar->Implements<UCombatInterface>())
			{
				ICombatInterface::Execute_AddOrUpdateWarpTargetFromLocation(Avatar, Target->GetActorLocation());
			}
		}
		// 更新攻击索引
		CurrentAttackIndex++;
	}
	// else
	// {
	// 	// 目标无效，跳过并继续
	// 	CurrentAttackIndex++;
	// 	ExecuteSingleAttack(); // 递归调用以继续攻击下一个目标
	// }
}

// 抛弃移动任务策略
// void UGA_ChainAttack::OnReachedCenter()
// {
// 	if (MoveTask)
// 	{
// 		MoveTask->OnTargetLocationReached.RemoveDynamic(this, &UGA_ChainAttack::OnReachedCenter);
// 		MoveTask = nullptr; // 清空，避免再次触发
// 	}
// 	
// 	AActor* AvatarActor = GetAvatarActorFromActorInfo();
// 	if (!AvatarActor)
// 	{
// 		K2_EndAbility();
// 	}
// 	// 设置检测对象类型为Pawn
// 	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
// 	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 只检测Pawn类型
// 	TArray<FHitResult> HitResults; // 用于存储命中结果
// 	TArray<AActor*> IgnoredActors; // 忽略的Actor
// 	IgnoredActors.Add(AvatarActor);// 忽略自身
// 	EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::ForDuration;// : EDrawDebugTrace::None;
// 	
// 	// 球形多重检测，查找路径上的所有目标
// 	UKismetSystemLibrary::SphereTraceMultiForObjects(
// 		AvatarActor, Center, Center, TargetRange, 
// 		ObjectTypes, false, IgnoredActors, DrawDebugTrace, HitResults, false);
// 	
// 	TSet<AActor*> HitTargetActors;
// 	// 获取角色的队伍接口
// 	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(AvatarActor);
// 	for (const FHitResult& OverlapResult : HitResults)
// 	{
// 		AActor* HitActor = OverlapResult.GetActor();
// 		if (!HitActor)
// 			continue;
// 		// 检查目标阵营关系（如只攻击敌人）
// 		if (OwnerTeamInterface)
// 		{
// 			if (OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor) != ETeamAttitude::Hostile)
// 			{
// 				continue;
// 			}
// 		}
// 	
// 		// 添加目标
// 		HitTargetActors.Add(OverlapResult.GetActor());
// 			
// 		// 在每个敌人身上触发GameplayCue特效
// 		if (AttackLocalGameplayCueTag.IsValid())
// 		{
// 			// 创建GameplayCue参数
// 			FGameplayCueParameters CueParameters;
// 			CueParameters.Location = HitActor->GetActorLocation();
// 			CueParameters.Normal = FVector::UpVector;
// 			// 如果目标没有实现GameplayCueInterface，则使用全局GameplayCue管理器
// 			UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
// 				HitActor, 
// 				AttackLocalGameplayCueTag, 
// 				EGameplayCueEvent::Executed, 
// 				CueParameters
// 			);
// 			
// 			// FGameplayCueParameters BlastingGameplayCueParameters;
// 			// // 设置特效的位置
// 			// BlastingGameplayCueParameters.Location = HitActor->GetActorLocation();
// 			// BlastingGameplayCueParameters.Normal = FVector::UpVector;
// 			// // 播放冲击特效和摄像机震动
// 			// GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(AttackLocalGameplayCueTag, BlastingGameplayCueParameters);
// 			// GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), BlastingGameplayCueParameters);
// 		}
// 		
// 	}
// 	// 创建目标数据
// 	FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(HitTargetActors.Array(), false);
// 	
// 	// 对范围内的所有敌人应用最后一击伤害
// 	if (TargetDataHandle.Num() > 0 && K2_HasAuthority())
// 	{
// 		ApplyDamageToTargetDataHandle(TargetDataHandle, LastHitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
// 		PushTargetsFromOwnerLocation(TargetDataHandle, TargetHitPushSpeed);
// 	}
// 	
// 	K2_EndAbility();
// }

void UGA_ChainAttack::ExecuteLastHit()
{
	StopAttacks();
	// // 仅在服务器有权限时播放动画
	// if (HasAuthority(&CurrentActivationInfo))
	// {
	// 	// 服务器播放动画
	// 	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	// 	PlayAttackMontage->ReadyForActivation();
	// }
	// else
	// {
	// 	PlayMontageLocally(AttackMontage);
	// }
	
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (!OwnerAnimInst) return;
	// 设置蒙太奇自动切换到下一个片段，达成连击的效果
	OwnerAnimInst->Montage_SetNextSection(OwnerAnimInst->Montage_GetCurrentSection(ChargeMontage), LastHitName, ChargeMontage);
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		K2_EndAbility();
	}
	// 更新目标位置
	if (AvatarActor->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_AddOrUpdateWarpTargetFromLocation(AvatarActor, Center);
	}

	// // 设置检测对象类型为Pawn
	// TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 只检测Pawn类型
	// TArray<FHitResult> HitResults; // 用于存储命中结果
	// TArray<AActor*> IgnoredActors; // 忽略的Actor
	// IgnoredActors.Add(AvatarActor);// 忽略自身
	// EDrawDebugTrace::Type DrawDebugTrace = EDrawDebugTrace::ForDuration;// : EDrawDebugTrace::None;
	//
	// // 球形多重检测，查找路径上的所有目标
	// UKismetSystemLibrary::SphereTraceMultiForObjects(
	// 	AvatarActor, Center, Center, TargetRange, 
	// 	ObjectTypes, false, IgnoredActors, DrawDebugTrace, HitResults, false);
	//
	// TSet<AActor*> HitTargetActors;
	// // 获取角色的队伍接口
	// const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(AvatarActor);
	// for (const FHitResult& OverlapResult : HitResults)
	// {
	// 	AActor* HitActor = OverlapResult.GetActor();
	// 	if (!HitActor)
	// 		continue;
	// 	// 检查目标阵营关系（如只攻击敌人）
	// 	if (OwnerTeamInterface)
	// 	{
	// 		if (OwnerTeamInterface->GetTeamAttitudeTowards(*HitActor) != ETeamAttitude::Hostile)
	// 		{
	// 			continue;
	// 		}
	// 	}
	//
	// 	// 添加目标
	// 	HitTargetActors.Add(OverlapResult.GetActor());
	// 		
	// 	// 在每个敌人身上触发GameplayCue特效
	// 	if (AttackLocalGameplayCueTag.IsValid())
	// 	{
	// 		// 创建GameplayCue参数
	// 		FGameplayCueParameters CueParameters;
	// 		CueParameters.Location = HitActor->GetActorLocation();
	// 		CueParameters.Normal = FVector::UpVector;
	// 		// 如果目标没有实现GameplayCueInterface，则使用全局GameplayCue管理器
	// 		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
	// 			HitActor, 
	// 			AttackLocalGameplayCueTag, 
	// 			EGameplayCueEvent::Executed, 
	// 			CueParameters
	// 		);
	// 		
	// 		// FGameplayCueParameters BlastingGameplayCueParameters;
	// 		// // 设置特效的位置
	// 		// BlastingGameplayCueParameters.Location = HitActor->GetActorLocation();
	// 		// BlastingGameplayCueParameters.Normal = FVector::UpVector;
	// 		// // 播放冲击特效和摄像机震动
	// 		// GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(AttackLocalGameplayCueTag, BlastingGameplayCueParameters);
	// 		// GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UCAbilitySystemStatics::GetCameraShakeGameplayCueTag(), BlastingGameplayCueParameters);
	// 	}
	// }
	// // 对范围内的所有敌人应用最后一击伤害
	// if (K2_HasAuthority())
	// {
	// 	// 创建目标数据
	// 	FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(HitTargetActors.Array(), false);
	// 	ApplyDamageToTargetDataHandle(TargetDataHandle, LastHitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	// 	PushTargetsFromOwnerLocation(TargetDataHandle, TargetHitPushSpeed);
	// }
	
	// 在服务端监听伤害事件
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
		WaitDamageEvent->EventReceived.AddDynamic(this, &UGA_ChainAttack::ExecuteLastHitDamage);
		WaitDamageEvent->ReadyForActivation();
	}
	// 抛弃移动任务
	// // 客户端预测平滑移动
	// MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(
	// 		this,
	// 		NAME_None,
	// 		Center,		// 目标点
	// 		0.3f,		// 持续 0.3 秒
	// 		nullptr,	// 插值曲线（线性）
	// 		nullptr		// 向量插值曲线
	// 	);
	// if (MoveTask)
	// {
	// 	MoveTask->OnTargetLocationReached.AddDynamic(this, &UGA_ChainAttack::OnReachedCenter);
	// 	MoveTask->ReadyForActivation();
	// }
	// if (K2_HasAuthority())
	// {
	// 	// 服务端瞬移到 Center
	// 	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	// 	{
	// 		AvatarActor->SetActorLocation(Center, false, nullptr, ETeleportType::TeleportPhysics);
	// 	}
	// 	// 服务端直接执行最终一击
	// 	OnReachedCenter();
	// }
	// else
	// {
	// 	// 客户端预测平滑移动
	// 	MoveTask = UAbilityTask_MoveToLocation::MoveToLocation(
	// 		this,
	// 		NAME_None,
	// 		Center,		// 目标点
	// 		0.3f,		// 持续 0.3 秒
	// 		nullptr,	// 插值曲线（线性）
	// 		nullptr		// 向量插值曲线
	// 	);
	// 	if (MoveTask)
	// 	{
	// 		MoveTask->OnTargetLocationReached.AddDynamic(this, &UGA_ChainAttack::OnReachedCenter);
	// 		MoveTask->ReadyForActivation();
	// 	}
	// }
}

void UGA_ChainAttack::ExecuteLastHitDamage(FGameplayEventData Payload)
{
	// 服务器中执行
	if (K2_HasAuthority())
	{
		// 获取目标数据
		FGameplayAbilityTargetDataHandle TargetDataHandle = Payload.TargetData;
		// 应用伤害
		ApplyDamageToTargetDataHandle(TargetDataHandle, LastHitDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		// 击退目标
		PushTargetsFromOwnerLocation(TargetDataHandle, TargetHitPushSpeed);
	}
}


void UGA_ChainAttack::StopAttacks()
{
	// 清除定时器
	if (GetWorld() && ChainAttackTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ChainAttackTimerHandle);
	}
	
	// 清空目标列表
	TargetActors.Empty();
	CurrentAttackIndex = 0;
}
