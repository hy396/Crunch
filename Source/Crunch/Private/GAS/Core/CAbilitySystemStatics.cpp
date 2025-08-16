// 幻雨喜欢小猫咪


#include "GAS/Core/CAbilitySystemStatics.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "CGameplayAbility.h"
#include "CGameplayAbilityTypes.h"
#include "CHeroAttributeSet.h"
#include "GameplayCueManager.h"
#include "TGameplayTags.h"

FGameplayTag UCAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	// 如果不喜欢用代码创建tag可以用下列方法，感觉有打错字的风险
	// 可以使用这种方法来获取到tag
	//return FGameplayTag::RequestGameplayTag("ability.basic attack");

	// 在cpp函数中定义的tag的获取方法如下
	return TGameplayTags::Ability_BasicAttack;
}

FGameplayTag UCAbilitySystemStatics::GetCameraShakeGameplayCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");
}

FGameplayTag UCAbilitySystemStatics::GetDamageNumberGameplayCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Number");
}

bool UCAbilitySystemStatics::IsActorDead(const AActor* ActorToCheck)
{
	return ActorHasTag(ActorToCheck, TGameplayTags::Stats_Dead);
}

bool UCAbilitySystemStatics::IsHero(const AActor* ActorToCheck)
{
	return ActorHasTag(ActorToCheck, TGameplayTags::Role_Hero);
}
bool UCAbilitySystemStatics::ActorHasTag(const AActor* ActorToCheck, const FGameplayTag& Tag)
{
	const IAbilitySystemInterface* ActorISA = Cast<IAbilitySystemInterface>(ActorToCheck);
	if (ActorISA)
	{
		UAbilitySystemComponent* ActorASC = ActorISA->GetAbilitySystemComponent();
		if (ActorASC)
		{
			return ActorASC->HasMatchingGameplayTag(Tag);
		}
	}
	return false;
}

bool UCAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec, const float PlayLevel)
{
	float MaxAbilityLevel;
	// 如果是大招进来了
	if (Spec.InputID == static_cast<int32>(ECAbilityInputID::AbilityR))
	{
		// 6~10 : 1
		// 11~15 : 2
		// 16~18 : 3
		MaxAbilityLevel = PlayLevel >= 16 ? 3 :
				   PlayLevel >= 11 ? 2 :
				   PlayLevel >= 6 ? 1 :
					0;
		//UE_LOG(LogTemp, Warning, TEXT("IsAbilityAtMaxLevelR: %f"), MaxAbilityLevel)

	}else
	{
		// Q、E、F的小技能
		/*
		* 1 ~ 2 ：1
		* 3 ~ 4 ：2
		* 5 ~ 6 ：3
		* 7 ~ 8 ：4
		* 9 ~18：5
		 */
		MaxAbilityLevel = PlayLevel >= 9 ? 5 :
					PlayLevel >= 7 ? 4 :
					PlayLevel >= 5 ? 3 :
					PlayLevel >= 3 ? 2 :
					1;
		//UE_LOG(LogTemp, Warning, TEXT("IsAbilityAtMaxLevelQEF: %f"), MaxAbilityLevel)

	}
	// Spec.InputID
	return Spec.Level >= MaxAbilityLevel;
}

float UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	// 获取冷却效果
	const UGameplayEffect* CoolDownEffect = Ability->GetCooldownGameplayEffect();
	if (!CoolDownEffect) return 0.f;

	float CooldownDuration = 0.f;
	// 调用GetStaticMagnitudeIfPossible方法从冷却效果中获取静态持续时间值
	CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
	return CooldownDuration;
}

float UCAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	// 获取消耗效果
	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect || CostEffect->Modifiers.Num() == 0) return 0.f;

	float Cost = 0.f;
	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
	// 取正值
	return FMath::Abs(Cost);
}

bool UCAbilitySystemStatics::CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec,
	const UAbilitySystemComponent& ASC)
{
	// 获取技能
	const UGameplayAbility* AbilityCDO = AbilitySpec.Ability;
	if (AbilityCDO)
	{
		// 调用技能的检查消耗方法
		return AbilityCDO->CheckCost(AbilitySpec.Handle, ASC.AbilityActorInfo.Get());
	}
	// 技能无效
	return false;
}

bool UCAbilitySystemStatics::CheckAbilityCostStatic(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC)
{
	if (AbilityCDO)
	{
		// 使用空句柄调用检查（适用于未实例化的技能）
		return AbilityCDO->CheckCost(FGameplayAbilitySpecHandle(), ASC.AbilityActorInfo.Get());
		// GetAbilityLevel(Handle, ActorInfo)失败
	}

	return false;  // 无有效技能对象时默认返回false
}

float UCAbilitySystemStatics::GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC,
	int AbilityLevel)
{
	float ManaCost = 0.f;
	if (AbilityCDO)
	{
		// 获取消耗效果
		UGameplayEffect* CostEffect = AbilityCDO->GetCostGameplayEffect();
		if (CostEffect && CostEffect->Modifiers.Num() > 0)
		{
			// 创建临时的效果规格
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(
				CostEffect->GetClass(), 
				AbilityLevel, 
				ASC.MakeEffectContext()
			);
			
			// 获取技能的消耗效果的静态效果值
			CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(
				*EffectSpec.Data.Get(),
				ManaCost
				);
		}
	}
	// 返回绝对值（确保消耗值始终为正数）
	return FMath::Abs(ManaCost);
}

float UCAbilitySystemStatics::GetCooldownDurationFor(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		// 获取技能关联的冷却效果
		UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
		if (CooldownEffect)
		{
			// 创建临时的效果规格
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(
				CooldownEffect->GetClass(), 
				AbilityLevel, 
				ASC.MakeEffectContext()
			);
	           //CustomMagnitude
			// 计算冷却效果的实际持续时间（考虑冷却缩减属性）
			CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(
				*EffectSpec.Data.Get(), 
				CooldownDuration
			);
		}
	}

	// 返回绝对值（确保冷却时间始终为正数）
	return FMath::Abs(CooldownDuration);
}

float UCAbilitySystemStatics::GetCooldownDurationForMMCCD(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		const UCGameplayAbility* Ability = Cast<UCGameplayAbility>(AbilityCDO);
		if (!Ability) return CooldownDuration;
		
		// 获取基础冷却时间
		float BaseCooldown = Ability->CooldownDuration.GetValueAtLevel(AbilityLevel);

		// 获取冷却缩减属性值
		bool bFound;
		float CooldownReduction = ASC.GetGameplayAttributeValue(UCHeroAttributeSet::GetCooldownReductionAttribute(), bFound);
		if (bFound)
		{
			// 计算最终冷却时间
			CooldownDuration = BaseCooldown * (1.0f - CooldownReduction/100.0f);
		}
	}

	// 返回绝对值（确保冷却时间始终为正数）
	return FMath::Abs(CooldownDuration);
}

float UCAbilitySystemStatics::GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO,
                                                      const UAbilitySystemComponent& ASC)
{
	if (!AbilityCDO) return 0.f;

	// 获取冷却效果
	UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0.f;

	// 创建查询条件：查找此技能对应的冷却效果
	FGameplayEffectQuery CooldownEffectQuery;
	CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();

	float CooldownRemaining = 0.f;
	// 获取所有匹配效果的剩余时间
	TArray<float> CooldownRemainings = ASC.GetActiveEffectsTimeRemaining(CooldownEffectQuery);
	// 找出最长的剩余时间
	for (float Remaining : CooldownRemainings)
	{
		if (Remaining > CooldownRemaining)
		{
			CooldownRemaining = Remaining;
		}
	}
	return CooldownRemaining;
}

void UCAbilitySystemStatics::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult,
	const FGameplayTag& GameplayCueTag)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, GameplayCueTag, EGameplayCueEvent::Executed, CueParams);
}
