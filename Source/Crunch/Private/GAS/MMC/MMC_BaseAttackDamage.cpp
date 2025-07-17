// 幻雨喜欢小猫咪


#include "GAS/MMC/MMC_BaseAttackDamage.h"

#include "GAS/Core/CAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	// 捕获伤害属性
	DamageCaptureDef.AttributeToCapture = UCAttributeSet::GetAttackDamageAttribute();
	// 属性来源指定为释放者
	DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	// 捕获目标的护甲属性
	ArmorCaptureDef.AttributeToCapture = UCAttributeSet::GetArmorAttribute();
	ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	// 添加捕获属性
	RelevantAttributesToCapture.Add(DamageCaptureDef);
	RelevantAttributesToCapture.Add(ArmorCaptureDef);
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvalParams;
	// 绑定源/目标标签
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	float AttackDamage = 0.f;
	// 获取攻击力属性值（通过DamageCaptureDef定义的捕获规则）
	GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvalParams, AttackDamage);	// 获取源的属性值

	float Armor = 0.f;
	// 获取目标护甲属性值（通过ArmorCaptureDef定义的捕获规则）
	GetCapturedAttributeMagnitude(ArmorCaptureDef, Spec, EvalParams, Armor);	// 获取目标的属性值

	// 计算最终伤害
	// 公式：Damage = AttackDamage * (1 - Armor / (Armor + 100))
	// 护甲减伤率 = Armor / (Armor + 100)
	float Damage = AttackDamage * (1 - Armor / (Armor + 100));

	return -Damage;
}
