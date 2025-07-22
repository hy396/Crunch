// 幻雨喜欢小猫咪


#include "GAS/MMC/MMC_LevelBased.h"

#include "GAS/Core/CHeroAttributeSet.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDefinition.AttributeToCapture = UCHeroAttributeSet::GetLevelAttribute();   // 捕获目标等级属性
	// 设置捕获对象，这里设置目标还是源都无所谓，因为是自己给自己
	LevelCaptureDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	// RateCaptureDefinition.AttributeToCapture = RateAttribute;
	// RateCaptureDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	// 注册捕获属性
	RelevantAttributesToCapture.Add(LevelCaptureDefinition);
	//RelevantAttributesToCapture.Add(RateCaptureDefinition);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 获取ASC，用来获取属性
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC) return 0.f;

	float Level = 0.f;
	// 设置评估参数
	FAggregatorEvaluateParameters EvalParams;
	// 绑定源/目标标签
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 获取目标对象的等级属性值
	GetCapturedAttributeMagnitude(LevelCaptureDefinition, Spec, EvalParams, Level);

	//float Rate = 0.f;
	//GetCapturedAttributeMagnitude(RateCaptureDefinition, Spec, EvalParams, Rate);
	// 获取预设的成长属性值
	bool bFound;
	float RateAttributeVal = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound)
		return 0.f;
	
	return (Level - 1) * RateAttributeVal;
}
