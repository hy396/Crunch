// 幻雨喜欢小猫咪


#include "GAS/Core/CGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "TGameplayTags.h"
#include "GameFramework/Character.h"
#include "GAS/Abilities/GAP_Launched.h"
#include "Kismet/KismetSystemLibrary.h"

UCGameplayAbility::UCGameplayAbility()
{
	// 眩晕状态无法激活技能
	ActivationBlockedTags.AddTag(TGameplayTags::Stats_Stun);
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

ACharacter* UCGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
	{
		// 从Actor信息中获取并缓存角色指针
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	}
	return AvatarCharacter;
}
