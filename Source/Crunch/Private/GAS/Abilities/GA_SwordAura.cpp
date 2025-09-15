// 幻雨喜欢小猫咪


#include "GA_SwordAura.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGA_SwordAura::UGA_SwordAura()
{
	// 技能激活时添加瞄准以及准星
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Aim);
	ActivationOwnedTags.AddTag(TGameplayTags::Stats_Crosshair);
}

void UGA_SwordAura::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	// 仅在服务器或有预测权限时绑定事件
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{

		UAbilityTask_PlayMontageAndWait* PlaySwingMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			SwingMontage
		);
		// 绑定动画结束/中断事件到能力结束
		PlaySwingMontage->OnBlendOut.AddDynamic(this, &UGA_SwordAura::K2_EndAbility);
		PlaySwingMontage->OnCancelled.AddDynamic(this, &UGA_SwordAura::K2_EndAbility);
		PlaySwingMontage->OnInterrupted.AddDynamic(this, &UGA_SwordAura::K2_EndAbility);
		PlaySwingMontage->OnCompleted.AddDynamic(this, &UGA_SwordAura::K2_EndAbility);
		PlaySwingMontage->ReadyForActivation();
		
		// 绑定发射剑气事件
		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Shoot, nullptr, false, false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &UGA_SwordAura::SpawnSwordAura);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UGA_SwordAura::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SwordAuraCountSpawned = 0;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_SwordAura::SpawnSwordAura(FGameplayEventData Payload)
{
	// 仅在服务器有权限时发射子弹
	if (K2_HasAuthority())
	{
		// 获取拥有者Actor
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerAvatarActor;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		// 计算剑气生成位置和方向
		FVector SpawnLocation;
		FRotator SpawnRotation;
		CalculateSwordAuraSpawnParams(SpawnLocation, SpawnRotation);
		// 生成剑气
		AProjectileActor* ProjectileActor = GetWorld()->SpawnActor<AProjectileActor>(
			SwordAuraProjectileClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams);
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
			
			// 初始化投射物
			ProjectileActor->ShootProjectile(
				SwordAuraSpeed,
				SwordAuraMaxDistance,
				nullptr, // 不追踪特定目标
				GetOwnerTeamId(),
				ASC->MakeOutgoingSpec(SwordAuraDamageEffect.DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo), EffectContextHandle),
				SwordAuraDamageEffect
			);
			// 剑气数量加1, 如果已生成数量达到上限，则结束技能
			SwordAuraCountSpawned++;
			if (SwordAuraCountSpawned >= SwordAuraCount)
			{
				K2_EndAbility();
			}
		}
	}
}

void UGA_SwordAura::CalculateSwordAuraSpawnParams(FVector& OutSpawnLocation, FRotator& OutSpawnRotation)
{
	// 默认位置和旋转
	OutSpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	OutSpawnRotation = GetAvatarActorFromActorInfo()->GetActorRotation();

	// 获取骨骼组件
	USkeletalMeshComponent* SkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (!SkeletalMeshComp)
	{
		return;
	}

	if (SocketNames.Num() <= SocketPositionIndex)
	{
		SocketPositionIndex = 0;
	}

	const FSwordSocketPair& SocketPair = SocketNames[SocketPositionIndex++];
	FVector StartSocketLocation = SkeletalMeshComp->GetSocketLocation(SocketPair.StartSocket);
	FVector EndSocketLocation   = SkeletalMeshComp->GetSocketLocation(SocketPair.EndSocket);

	// ---------------- 核心方向计算 ----------------
	// 前进方向：角色的朝向
	FRotator AvatarRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
	FVector Forward = AvatarRotation.Vector().GetSafeNormal();

	// 横向方向：武器 Start-End 向量
	FVector Right = (EndSocketLocation - StartSocketLocation).GetSafeNormal();

	// 避免和 Forward 平行，检查一次
	if (FMath::Abs(FVector::DotProduct(Forward, Right)) > 0.95f)
	{
		// 太接近时，强行改成角色自身的右方向
		Right = FRotationMatrix(AvatarRotation).GetScaledAxis(EAxis::Y);
	}

	// 上方向 = Forward × Right
	FVector Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();

	// 再修正 Right 保证正交
	Right = FVector::CrossProduct(Up, Forward).GetSafeNormal();

	// 构建旋转矩阵
	FMatrix SwordAuraMatrix(Forward, Right, Up, FVector::ZeroVector);

	// 转 Rotator
	OutSpawnRotation = SwordAuraMatrix.Rotator();

	// ---------------- 位置 ----------------
	OutSpawnLocation = (StartSocketLocation + EndSocketLocation) * 0.5f;

	
	// 计算武器向量
	// FVector WeaponDirection = (EndSocketLocation - StartSocketLocation).GetSafeNormal();
	// // 计算角色朝向
	// FRotator AvatarRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
	// FVector AvatarForward = AvatarRotation.Vector();
 //    
	// // 将武器向量投影到角色的右向量上，以确定在武器上的位置偏移
	// FVector AvatarRight = FRotationMatrix(AvatarRotation).GetUnitAxis(EAxis::Y);
	// float Projection = FVector::DotProduct(WeaponDirection, AvatarRight);
 //    
	// // 根据投影值在武器线上选择生成点，使剑气沿着角色朝向发射
	// OutSpawnLocation = FMath::Lerp(StartSocketLocation, EndSocketLocation, (Projection + 1.0f) * 0.5f);
	//
	// // 使用角色的朝向作为剑气的发射方向
	// OutSpawnRotation = AvatarRotation;

	// // 计算中点作为生成位置
	// OutSpawnLocation = (StartSocketLocation + EndSocketLocation) / 2.0f;
	//
	// // 使用角色的朝向作为剑气的发射方向
	// OutSpawnRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
}
