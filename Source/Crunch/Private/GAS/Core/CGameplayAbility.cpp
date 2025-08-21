// 幻雨喜欢小猫咪


#include "GAS/Core/CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CAttributeSet.h"
#include "TGameplayTags.h"
#include "GameFramework/Character.h"
#include "GAS/Abilities/GAP_Launched.h"
#include "Kismet/KismetSystemLibrary.h"
// #include "Net/UnrealNetwork.h"

UCGameplayAbility::UCGameplayAbility()
{
	// 眩晕状态无法激活技能
	ActivationBlockedTags.AddTag(TGameplayTags::Stats_Stun);
}

bool UCGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec && AbilitySpec->Level <= 0)
	{
		return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

AActor* UCGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
	// 获取当前执行能力的角色
	AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (OwnerAvatarActor)
	{
		// 获取角色的视觉位置和视角方向
		FVector Location;
		FRotator Rotation;
		OwnerAvatarActor->GetActorEyesViewPoint(Location, Rotation);
		
		// 计算瞄准射线的终点
		FVector AimEnd = Location + Rotation.Vector() * AimDistance;

		// 设置碰撞查询参数
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(OwnerAvatarActor); // 忽略自身
        
		// 设置碰撞对象查询参数（只查询Pawn类型对象）
		FCollisionObjectQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn); // 仅检测Pawn对象

		// 调试模式：绘制瞄准线
		if (ShouldDrawDebug())
		{
			DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);
		}
		// 射线检测
		TArray<FHitResult> HitResults;
		if (GetWorld()->LineTraceMultiByObjectType(
			HitResults,
			Location,
			AimEnd,
			CollisionObjectQueryParams,
			CollisionQueryParams))
		{
			// 遍历命中结果
			for (FHitResult& HitResult : HitResults)
			{
				// 寻找指定阵容的Actor
				if (IsActorTeamAttitudeIs(HitResult.GetActor(), TeamAttitude))
				{
					// 返回命中的Actor
					return HitResult.GetActor();
				}
			}
		}
	}
	return nullptr;
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeam,
	bool bDrawDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults; // 存储最终返回的命中结果
	TSet<AActor*> HitActors;      // 记录已命中的Actor，避免重复

	// 获取拥有者(Avatar)的队伍接口
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	// 遍历所有目标数据
	for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
	{
		// 获取扫描起点和终点
		FVector StartLoc = TargetData->GetOrigin().GetTranslation();
		FVector EndLoc = TargetData->GetEndPoint();

		// 设置要检测的对象类型(这里只检测Pawn)
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		// 设置要忽略的Actor(如果需要忽略自己)
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		// 设置调试绘制模式
		EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		// 执行球形多对象扫描
		TArray<FHitResult> Results;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false, ActorsToIgnore, DrawDebugTrace, Results, false);
	
		// 处理扫描结果
		for (const FHitResult& Result : Results)
		{
			// 如果已经命中过该Actor，则跳过
			if (HitActors.Contains(Result.GetActor()))
			{
				continue;
			}

			// 检查队伍关系(如果设置了队伍过滤)
			if (OwnerTeamInterface)
			{
				ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor());
				if (OtherActorTeamAttitude != TargetTeam)
				{
					continue;
				}
			}

			// 记录命中的Actor并添加结果
			HitActors.Add(Result.GetActor());
			OutResults.Add(Result);
		}
	}
	
	return OutResults; // 返回所有有效命中结果
}

void UCGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult,
	TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	// if (!GameplayEffect) return;
	// 创建一个传出游戏效果规范句柄，包含指定的GameplayEffect和等级
	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, Level);

	// 创建一个游戏效果上下文句柄，包含当前能力规范句柄和当前Actor信息
	FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());

	// 向上下文中添加命中结果信息
	EffectContext.AddHitResult(HitResult);

	// 将上下文设置到效果规范数据中
	EffectSpecHandle.Data->SetContext(EffectContext);

	// 在目标上应用游戏效果规范
	ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(),
									GetCurrentActorInfo(),
									GetCurrentActivationInfo(),
									EffectSpecHandle,
									UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
}

void UCGameplayAbility::ApplyDamageToActor(AActor* TargetActor,const FGenericDamageEffectDef& Damage, int Level)
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AActor* AvatarActor				   = GetAvatarActorFromActorInfo();
	// 创建效果上下文， 设置能力 、源对象 和 施加者
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(AvatarActor);
	ContextHandle.AddInstigator(AvatarActor, AvatarActor);
	// // 创建效果Spec句柄，指定效果类、能力等级和上下文
	// FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Damage.DamageEffect, Level, ContextHandle);
	
	// 配置伤害
	// MakeDamage(Damage, Level);
	for (const auto& TypePair : Damage.DamageTypeDefinitions)
	{
		// 创建效果Spec句柄，指定效果类、能力等级和上下文
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Damage.DamageEffect, Level, ContextHandle);
		float TotalModifier = TypePair.Value.BaseDamage.GetValueAtLevel(Level);
		for (const auto& Modifier : TypePair.Value.AttributeDamageModifiers)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Modifier.Key, Modifier.Value);
		}
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, TypePair.Key, TotalModifier);
		// 在目标上应用游戏效果规范
		ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(),
										GetCurrentActorInfo(),
										GetCurrentActivationInfo(),
										EffectSpecHandle,
										UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor));
	}
	// // 在目标上应用游戏效果规范
	// ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(),
	// 								GetCurrentActorInfo(),
	// 								GetCurrentActivationInfo(),
	// 								EffectSpecHandle,
	// 								UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor));
}

void UCGameplayAbility::ApplyDamageToTargetDataHandle(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	const FGenericDamageEffectDef& Damage, int Level)
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AActor* AvatarActor				   = GetAvatarActorFromActorInfo();
	// 创建效果上下文， 设置能力 、源对象 和 施加者
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(AvatarActor);
	ContextHandle.AddInstigator(AvatarActor, AvatarActor);

	for (const auto& TypePair : Damage.DamageTypeDefinitions)
	{
		// 创建效果Spec句柄，指定效果类、能力等级和上下文
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Damage.DamageEffect, Level, ContextHandle);
		float TotalModifier = TypePair.Value.BaseDamage.GetValueAtLevel(Level);
		for (const auto& Modifier : TypePair.Value.AttributeDamageModifiers)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Modifier.Key, Modifier.Value);
		}
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, TypePair.Key, TotalModifier);
		// 在目标上应用游戏效果规范
		K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle,TargetDataHandle);
	}
}

void UCGameplayAbility::MakeDamage(const FGenericDamageEffectDef& Damage, int Level)
{
	// TODO:废弃方案，但有点感情，没删
	// 通通置为0
	// float BaseAttackDamage = 0.f;
	// float BaseMagicDamage = 0.f;
	// float BaseTrueDamage = 0.f;
	// for (const auto& TypePair : Damage.DamageTypeDefinitions)
	// {
	// 	float TotalModifier = TypePair.Value.BaseDamage.GetValueAtLevel(Level);
	// 	for (const auto& Modifier : TypePair.Value.AttributeDamageModifiers)
	// 	{
	// 		bool bFound ;
	// 		float AttributeValue = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(Modifier.Key, bFound);
	// 		if (bFound)
	// 		{
	// 			TotalModifier += AttributeValue * Modifier.Value / 100.0f;
	// 		}
	// 	}
	// 	
	// 	// switch (TypePair.Key)
	// 	// {
	// 	// 	case ETDamageType::PhysicalDamage :
	// 	// 		BaseAttackDamage = TotalModifier;
	// 	// 		break;
	// 	// 	case ETDamageType::MagicDamage :
	// 	// 		BaseMagicDamage = TotalModifier;
	// 	// 		break;
	// 	// 	case ETDamageType::TrueDamage :
	// 	// 		BaseTrueDamage = TotalModifier;
	// 	// 		break;
	// 	// 	default:
	// 	// 		break;
	// 	// }
	// 	
	// }
	// GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseAttackDamageAttribute(), EGameplayModOp::Override, BaseAttackDamage);
	// GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseMagicDamageAttribute(), EGameplayModOp::Override, BaseMagicDamage);
	// GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseTrueDamageAttribute(), EGameplayModOp::Override, BaseTrueDamage);
}

void UCGameplayAbility::PushSelf(const FVector& PushVel)
{
	// 获取有效拥有者角色并执行击飞
	ACharacter* OwningAvatarCharacter = GetOwningAvatarCharacter();
	if (OwningAvatarCharacter)
	{
		OwningAvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel)
{
	// 目标为空则返回
	if (!Target) return;

	FGameplayEventData EventData;

	// 创建单目标命中数据对象
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit;
	// 配置命中结果参数
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVel; // 设置冲击方向为力的方向
	HitData->HitResult = HitResult;
	EventData.TargetData.Add(HitData);
	// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, TGameplayTags::Ability_Passive_Launch_Activate, EventData);
	if (Target->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
	{
		// 用标签激活技能
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, UGAP_Launched::GetLaunchedAbilityActivationTag(), EventData);
	}
}

void UCGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel)
{
	for(AActor* Target : Targets)
	{
		PushTarget(Target, PushVel);
	}
}

void UCGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargets(Targets, PushVel);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed)
{
	AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
	if (!OwnerAvatarActor)
		return;

	FVector OwnerAvatarActorLocation = OwnerAvatarActor->GetActorLocation();
	PushTargetsFromLocation(Targets, OwnerAvatarActorLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	float PushSpeed)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromOwnerLocation(TargetActors, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	const FVector& FromLocation, float PushSpeed)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromLocation(Targets, FromLocation, PushSpeed);
}

void UCGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation,
	float PushSpeed)
{
	for (AActor* Target : Targets)
	{
		FVector PushDir = Target->GetActorLocation() - FromLocation;
		PushDir.Z = 0;
		PushDir.Normalize();

		PushTarget(Target, PushDir * PushSpeed);
	}
}

void UCGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst && !OwnerAnimInst->Montage_IsPlaying(MontageToPlay))
	{
		OwnerAnimInst->Montage_Play(MontageToPlay);
	}
}

void UCGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (OwnerAnimInst)
	{
		FName CurrentSectionName = OwnerAnimInst->Montage_GetCurrentSection(MontageToStop);
		OwnerAnimInst->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
	}
}

FGenericTeamId UCGameplayAbility::GetOwnerTeamId() const
{
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamInterface)
	{
		return OwnerTeamInterface->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

bool UCGameplayAbility::IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
	if (!OtherActor)
		return false;

	IGenericTeamAgentInterface* OwnerTeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamAgentInterface)
	{
		return OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
	}

	return false;
}

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
	{
		// 从Actor信息中获取并缓存角色指针
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}
	return AvatarCharacter;
}

void UCGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC)
	{
		OwnerASC->HandleGameplayEvent(EventTag, &EventData);
	}
}

// void UCGameplayAbility::Client_SetAvatarMeshSocketLocation_Implementation(FName SocketName)
// {
// 	FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
// 	if (const USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo())
// 	{
// 		SocketLocation = MeshComp->GetSocketLocation(SocketName);
// 	}
// 	My_SocketLocation = SocketLocation;
// }

FVector UCGameplayAbility::GetAvatarMeshSocketLocation(const FName SocketName) const
{
	FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	if (const USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo())
	{
		SocketLocation = MeshComp->GetSocketLocation(SocketName);
	}
	return SocketLocation;
}