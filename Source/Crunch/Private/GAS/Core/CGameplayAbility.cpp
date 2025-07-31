// 幻雨喜欢小猫咪


#include "GAS/Core/CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CAttributeSet.h"
#include "TGameplayTags.h"
#include "GameFramework/Character.h"
#include "GAS/Abilities/GAP_Launched.h"
#include "Kismet/KismetSystemLibrary.h"

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

void UCGameplayAbility::ApplyDamage(AActor* TargetActor,const FGenericDamageEffectDef& Damage, int Level)
{
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	AActor* AvatarActor				   = GetAvatarActorFromActorInfo();
	// 创建效果上下文， 设置能力 、源对象 和 施加者
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(AvatarActor);
	ContextHandle.AddInstigator(AvatarActor, AvatarActor);
	// 配置伤害
	MakeDamage(Damage, Level);
	// float NewDamage = Damage.BaseDamage.GetValueAtLevel(GetAbilityLevel());
	// for(auto& Pair : Damage.DamageTypes)
	// {
	// 	bool bFound ;
	// 	float AttributeValue = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(Pair.Key, bFound);
	// 	if (bFound)
	// 	{
	// 		NewDamage += AttributeValue * Pair.Value / 100.f;
	// 	}
	// }
	// // 设置伤害的属性
	// GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseAttackDamageAttribute(), EGameplayModOp::Override, NewDamage);

	// GetAbilitySystemComponentFromActorInfo()->SetNumericAttributeBase(UCAttributeSet::GetBaseDamageAttribute(), NewDamage);
	
	// 创建效果Spec句柄，指定效果类、能力等级和上下文
	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(Damage.DamageEffect, Level, ContextHandle);
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, TGameplayTags::AttributeSet_BaseDamage, NewDamage);
	// UAbilitySystemBlueprintLibrary::AssignSetByCallerMagnitude(EffectSpecHandle, "CAttributeSet.BaseDamage", NewDamage);
	// if (EffectSpecHandle.IsValid())
	// {
	// 	// 设置由调用者指定的基础伤害数值
	// 	EffectSpecHandle.Data->SetSetByCallerMagnitude(TGameplayTags::AttributeSet_BaseDamage, NewDamage);
	// }
	// 在目标上应用游戏效果规范
	ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(),
									GetCurrentActorInfo(),
									GetCurrentActivationInfo(),
									EffectSpecHandle,
									UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor));
}

void UCGameplayAbility::MakeDamage(const FGenericDamageEffectDef& Damage, int Level)
{
	// 通通置为0
	float BaseAttackDamage = 0.f;
	float BaseMagicDamage = 0.f;
	float BaseTrueDamage = 0.f;
	for (const auto& TypePair : Damage.DamageTypeDefinitions)
	{
		float TotalModifier = TypePair.Value.BaseDamage.GetValueAtLevel(Level);
		for (const auto& Modifier : TypePair.Value.AttributeDamageModifiers)
		{
			bool bFound ;
			float AttributeValue = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(Modifier.Key, bFound);
			if (bFound)
			{
				TotalModifier += AttributeValue * Modifier.Value / 100.0f;
			}
		}
		switch (TypePair.Key)
		{
			case EDamageType::PhysicalDamage :
				BaseAttackDamage = TotalModifier;
				break;
			case EDamageType::MagicDamage :
				BaseMagicDamage = TotalModifier;
				break;
			case EDamageType::TrueDamage :
				BaseTrueDamage = TotalModifier;
				break;
			default:
				break;
		}
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseAttackDamageAttribute(), EGameplayModOp::Override, BaseAttackDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseMagicDamageAttribute(), EGameplayModOp::Override, BaseMagicDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyModToAttribute(UCAttributeSet::GetBaseTrueDamageAttribute(), EGameplayModOp::Override, BaseTrueDamage);
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
	// 用标签激活技能
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, UGAP_Launched::GetLaunchedAbilityActivationTag(), EventData);
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

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
	{
		// 从Actor信息中获取并缓存角色指针
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}
	return AvatarCharacter;
}
