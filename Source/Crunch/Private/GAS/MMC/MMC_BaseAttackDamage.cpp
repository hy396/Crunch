// 幻雨喜欢小猫咪


#include "GAS/MMC/MMC_BaseAttackDamage.h"

#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GAS/Core/CHeroAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	// // 捕获伤害属性
	// DamageCaptureDef.AttributeToCapture = UCAttributeSet::GetAttackDamageAttribute();
	// // 属性来源指定为释放者
	// DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	//
	// // 捕获目标的护甲属性
	// ArmorCaptureDef.AttributeToCapture = UCAttributeSet::GetArmorAttribute();
	// ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	// 获取冷却缩减属性
	CooldownReductionCaptureDef.AttributeToCapture = UCHeroAttributeSet::GetCooldownReductionAttribute();
	CooldownReductionCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	// 添加捕获属性
	// RelevantAttributesToCapture.Add(DamageCaptureDef);
	// RelevantAttributesToCapture.Add(ArmorCaptureDef);
	RelevantAttributesToCapture.Add(CooldownReductionCaptureDef);
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{


	
	FAggregatorEvaluateParameters EvalParams;
	// 绑定源/目标标签
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	// float AttackDamage = 0.f;
	// // 获取攻击力属性值（通过DamageCaptureDef定义的捕获规则）
	// GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvalParams, AttackDamage);	// 获取源的属性值
	//
	// float Armor = 0.f;
	// // 获取目标护甲属性值（通过ArmorCaptureDef定义的捕获规则）
	// GetCapturedAttributeMagnitude(ArmorCaptureDef, Spec, EvalParams, Armor);	// 获取目标的属性值
	//
	// // 计算最终伤害
	// // 公式：Damage = AttackDamage * (1 - Armor / (Armor + 100))
	// // 护甲减伤率 = Armor / (Armor + 100)
	// float Damage = AttackDamage * (1 - Armor / (Armor + 100));
	//
	//
	// return -Damage;

	// 获取Ability实例
	const UCGameplayAbility* Ability = Cast<UCGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());
	if (!Ability) return 0.0f;
	// 到CAbility中设置
	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	// FScalableFloat CooldownDuration;
	
	// 获取基础冷却时间
	float BaseCooldown = Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());

	// 获取冷却缩减属性值
	float CooldownReduction = 0.f;
	//（通过CooldownReductionCaptureDef定义的捕获规则）
	GetCapturedAttributeMagnitude(CooldownReductionCaptureDef, Spec, EvalParams, CooldownReduction);	// 获取源的属性值

	// 计算冷却
	float ActualCooldown = BaseCooldown * (1.0f - CooldownReduction/100.0f);

	return FMath::Max(0.1f, ActualCooldown);
}
