// 幻雨喜欢小猫咪


#include "GA_Shoot.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actor/ProjectileActor.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GameplayTagsManager.h"

UGA_Shoot::UGA_Shoot(): AimTargetAbilitySystemComponent(nullptr)
{
	// 技能激活时添加瞄准以及准星
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Aim);
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Crosshair);
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 提交消耗以及cd
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("激活射击技能"));

	// 仅在服务器或有预测权限时绑定事件
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 绑定开始射击事件
		UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_BasicAttack_Pressed);
		WaitStartShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StartShooting);
		WaitStartShootingEvent->ReadyForActivation();

		// 绑定停止射击事件
		UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_BasicAttack_Released);
		WaitStopShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StopShooting);
		WaitStopShootingEvent->ReadyForActivation();

		// 绑定发射子弹事件
		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Shoot, nullptr, false, false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &UGA_Shoot::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("停止射击技能"));
	K2_EndAbility();
}

void UGA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 移除目标死亡标签监听
	if (AimTargetAbilitySystemComponent)
	{
		AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).RemoveAll(this);
		AimTargetAbilitySystemComponent = nullptr;
	}
	// 通知目标已更新
	SendLocalGameplayEvent(TGameplayTags::Target_Updated, FGameplayEventData());

	
	// 停止射击
	StopShooting(FGameplayEventData());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Shoot::StartShooting(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("开始射击技能"));
	// 1. 网络同步任务：确保客户端-服务器同步
	// UAbilityTask_NetworkSyncPoint* NetWorkSync = UAbilityTask_NetworkSyncPoint::WaitNetSync(
	// 	this, 
	// 	EAbilityTaskNetSyncType::OnlyServerWait  // 仅服务器等待同步
	// );
	// NetWorkSync->ReadyForActivation();
	//
	// // 2. 创建并播放射击动画蒙太奇
	// UAbilityTask_PlayMontageAndWait* PlayShootMontage = 
	// 	UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	// 		this,
	// 		NAME_None,                 // 无特定任务名
	// 		ShootMontage               // 使用的动画蒙太奇资源
	// 	);
	// PlayShootMontage->ReadyForActivation();
	
	// 仅在服务器有权限时播放动画
	if (HasAuthority(&CurrentActivationInfo))
	{
		// 服务器播放射击动画
		UAbilityTask_PlayMontageAndWait* PlayShootMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		PlayShootMontage->ReadyForActivation();
		//UE_LOG(LogTemp, Warning, TEXT("服务器"))
	}
	else
	{
		// 客户端本地播放射击动画
		PlayMontageLocally(ShootMontage);
		//UE_LOG(LogTemp, Warning, TEXT("客户端"))
	}
	// 查找瞄准目标并启动检测定时器
	FindAimTarget();
	StartAimTargetCheckTimer();
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	if (ShootMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("停止射击技能"));
		// 停止蒙太奇动画
		StopMontageAfterCurrentSection(ShootMontage);
	}
	//TODO:退出的时候重新瞄准
	// 移除目标死亡标签监听
	// if (AimTargetAbilitySystemComponent)
	// {
	// 	AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).RemoveAll(this);
	// 	AimTargetAbilitySystemComponent = nullptr;
	// }
	// // 通知目标已更新
	// SendLocalGameplayEvent(TGameplayTags::Target_Updated, FGameplayEventData());
	
	StopAimTargetCheckTimer();
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("发射子弹"))
	// // 默认发射位置为角色位置
	// FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	// // 我从客户端中获取发射位置
	// UE_LOG(LogTemp, Warning, TEXT("发射位置1：%s"), *SocketLocation.ToString())
	// // 获取角色的骨骼
	// USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo();
	// if (MeshComp)
	// {
	// 	// 如果事件标签包含Socket名，则用Socket位置
	// 	TArray<FName> OutNames;
	// 	UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
	// 	if (OutNames.Num() != 0)
	// 	{
	// 		FName SocketName = OutNames.Last();
	// 		//UE_LOG(LogTemp, Warning, TEXT("SocketName：%s"), *SocketName.ToString())
	// 		SocketLocation = MeshComp->GetSocketLocation(SocketName);
	// 	}
	// }
	 if (!K2_HasAuthority())
	 {
	 	// 默认发射位置为角色位置
		// SocketLocation_0 = GetAvatarActorFromActorInfo()->GetActorLocation();
	 // 	// 我从客户端中获取发射位置
	 // 	UE_LOG(LogTemp, Warning, TEXT("发射位置1：%s"), *SocketLocation_0.ToString())
	 // 	// 获取角色的骨骼
	 // 	USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo();
	 // 	if (MeshComp)
	 // 	{
	 // 		// 如果事件标签包含Socket名，则用Socket位置
	 // 		TArray<FName> OutNames;
	 // 		UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
	 // 		if (OutNames.Num() != 0)
	 // 		{
	 // 			FName SocketName = OutNames.Last();
	 // 			//UE_LOG(LogTemp, Warning, TEXT("SocketName：%s"), *SocketName.ToString())
	 // 			SocketLocation_0 = MeshComp->GetSocketLocation(SocketName);
	 // 		}
	 // 	}
		// UE_LOG(LogTemp, Warning, TEXT("客户端的发射位置：%s"), *SocketLocation_0.ToString())
	 }
	// 仅在服务器有权限时发射子弹
	if (K2_HasAuthority())
	{
		// UE_LOG(LogTemp, Warning, TEXT("服务器的发射位置：%s"), *SocketLocation_0.ToString())
		// 获取拥有者Actor
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerAvatarActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 默认发射位置为角色位置
		FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		UE_LOG(LogTemp, Warning, TEXT("发射位置1：%s"), *SocketLocation.ToString())
		// 获取角色的骨骼
		USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo();
		if (MeshComp)
		{
			// TODO:服务器获取的数据在客户端运行下无法获取
			// 如果事件标签包含Socket名，则用Socket位置
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num() != 0)
			{
				FName SocketName = OutNames.Last();
				//UE_LOG(LogTemp, Warning, TEXT("SocketName：%s"), *SocketName.ToString())
				SocketLocation = MeshComp->GetSocketLocation(SocketName);
				UE_LOG(LogTemp, Warning, TEXT("发射位置2：%s"), *SocketLocation.ToString())
			}
		}
		// //TODO:获取一下传入的数据
		// if (Payload.ContextHandle.HasOrigin())
		// {
		// 	SocketLocation = Payload.ContextHandle.GetOrigin();
		// 	UE_LOG(LogTemp, Warning, TEXT("发射位置2：%s"), *SocketLocation.ToString())
		// }

		// 生成子弹
		AProjectileActor* ProjectileActor = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SocketLocation, OwnerAvatarActor->GetActorRotation(), SpawnParams);
		if (ProjectileActor)
		{
			const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
			AActor* AvatarActor				   = GetAvatarActorFromActorInfo();
			// 创建效果上下文， 设置能力 、源对象 和 施加者
			FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
			// 将施法者Actor添加到上下文源对象中（用于追踪效果来源）
			EffectContextHandle.AddSourceObject(AvatarActor);
			EffectContextHandle.SetAbility(this);
			EffectContextHandle.AddSourceObject(AvatarActor);
			EffectContextHandle.AddInstigator(AvatarActor, AvatarActor);

			ProjectileActor->ShootProjectile(
								ShootProjectileSpeed,
					ShootProjectileRange,
								GetAimTargetIfValid(),
								GetOwnerTeamId(),
								ASC->MakeOutgoingSpec(ProjectileHitEffect.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo), EffectContextHandle),
							ProjectileHitEffect
							);
		}
	}
}

AActor* UGA_Shoot::GetAimTargetIfValid() const
{
	if (HasValidTarget())
		return AimTarget;
	
	return nullptr;
}

void UGA_Shoot::FindAimTarget()
{
	// 已经有有效目标直接退出
	if (HasValidTarget())
		return;

	// 没有有效目标，曾经获取过目标ASC移除死亡监听
	if (AimTargetAbilitySystemComponent)
	{
		AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).RemoveAll(this);
		AimTargetAbilitySystemComponent = nullptr;
	}

	// 查找射程内的敌方目标
	AimTarget = GetAimTarget(ShootProjectileRange, ETeamAttitude::Hostile);
	if (AimTarget)
	{
		AimTargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AimTarget);
		if (AimTargetAbilitySystemComponent)
		{
			// 监听目标死亡标签变化
			AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).AddUObject(this, &UGA_Shoot::TargetDeadTagUpdated);
		}
	}
	// 通知目标已更新
	FGameplayEventData EventData;
	EventData.Target = AimTarget;
	SendLocalGameplayEvent(TGameplayTags::Target_Updated, EventData);
}

bool UGA_Shoot::HasValidTarget() const
{
	// 目标不存在
	if (!AimTarget)
		return false;

	// 角色死亡
	if (UCAbilitySystemStatics::IsActorDead(AimTarget))
		return false;
	
	// 目标不在范围内
	if (!IsTargetInRange())
		return false;

	return true;
}

void UGA_Shoot::StartAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(AimTargetCheckTimerHandle, this, &UGA_Shoot::FindAimTarget, AimTargetCheckTimeInterval, true);
	}
}

void UGA_Shoot::StopAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(AimTargetCheckTimerHandle);
	}
}

bool UGA_Shoot::IsTargetInRange() const
{
	if (!AimTarget) return false;
	// 获取目标距离
	float Distance = FVector::Distance(GetAvatarActorFromActorInfo()->GetActorLocation(), AimTarget->GetActorLocation());
	return Distance <= ShootProjectileRange;
}

void UGA_Shoot::TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FindAimTarget();
	}
}


