// 幻雨喜欢小猫咪


#include "GAS/Core/CAbilitySystemStatics.h"

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
