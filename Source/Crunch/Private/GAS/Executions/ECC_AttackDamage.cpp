// 幻雨喜欢小猫咪


#include "GAS/Executions/ECC_AttackDamage.h"

#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"

struct FDamageStatics
{
	// FGameplayEffectAttributeCaptureDefinition
	// 物理基础伤害
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseAttackDamage);
	// 魔法基础伤害
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseMagicDamage);
	// 真实基础伤害
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseTrueDamage);
	
	// 护甲穿透
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	// 护甲穿透百分比
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationPercent);

	// 法术穿透
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetration);
	// 法术穿透百分比
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetrationPercent);
	
	// 伤害加深
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageAmplification);
	// 敌方的物理防御
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	// 敌方的法术抗性
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
	// 伤害减免
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);

	FDamageStatics()
	{
	 	// 参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, BaseAttackDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, BaseMagicDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, BaseTrueDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, ArmorPenetrationPercent, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, MagicPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, MagicPenetrationPercent, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, DamageAmplification, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MagicResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, DamageReduction, Target, false);
	 }
};
// 静态数据访问函数（单例模式）
static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UECC_AttackDamage::UECC_AttackDamage()
{
	// 将属性添加到捕获列表中
	RelevantAttributesToCapture.Add(DamageStatics().BaseAttackDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseMagicDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseTrueDamageDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationPercentDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicPenetrationPercentDef);

	RelevantAttributesToCapture.Add(DamageStatics().DamageAmplificationDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);
}

void UECC_AttackDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 获取游戏效果规范和上下文
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	// 获取来源和目标标签
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 初始化评估参数
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// 获取伤害加深
	float DamageAmp = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageAmplificationDef, EvaluateParameters, DamageAmp);
	// 获取敌方的伤害减免
	float DamageReduction = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvaluateParameters, DamageReduction);
	
	// 计算基础物理伤害值
	float BaseAttackDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BaseAttackDamageDef, 
		EvaluateParameters, 
		BaseAttackDamage
	);

	// 物理伤害的处理
	if (BaseAttackDamage > 0.0f)
	{
		// 获取护甲穿透百分比
		float ArmorPenetrationPercent = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationPercentDef, EvaluateParameters, ArmorPenetrationPercent);
		// 获取护甲穿透
		float ArmorPenetration = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, ArmorPenetration);
		// 获取目标护甲
		float TargetArmor = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
		// 1. 处理固定护甲穿透
		TargetArmor = FMath::Max(0.0f, TargetArmor - ArmorPenetration);
		// 2. 处理百分比护甲穿透
		TargetArmor = FMath::Max(0.0f, TargetArmor * (1.0f - FMath::Min(ArmorPenetrationPercent, 100.0f) / 100.0f));
		// 3. 计算护甲减免（计算出来的是免伤率）
		float ArmorReduction = TargetArmor / (TargetArmor + 100.0f);
		BaseAttackDamage *= (1.0f - FMath::Min(ArmorReduction / 100.0f + DamageReduction/100.0f, 1.0f));
		// 4. 应用伤害加深（百分比提升）
		BaseAttackDamage *= (1.0f + DamageAmp / 100.0f);
		// 5. 输出到AttackDamage属性
		if (BaseAttackDamage > 0.0f)
		{
			// 添加输出修饰符
			OutExecutionOutput.AddOutputModifier(
				FGameplayModifierEvaluatedData(
				UCAttributeSet::GetAttackDamageAttribute(), //获取到伤害属性
				EGameplayModOp::Override, 
				BaseAttackDamage	//伤害
				));
		}
	}

	// 计算基础法术伤害值
	float BaseMagicDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BaseMagicDamageDef, 
		EvaluateParameters, 
		BaseMagicDamage
	);
	if (BaseMagicDamage > 0)
	{
		// 获取法术穿透百分比
		float MagicPenetrationPercent = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().MagicPenetrationPercentDef,
			EvaluateParameters, MagicPenetrationPercent);
		// 获取法术穿透
		float MagicPenetration = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().MagicPenetrationDef,
			EvaluateParameters, MagicPenetration);
		// 获取目标法抗
		float TargetMagicResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().MagicResistanceDef, EvaluateParameters, TargetMagicResistance);
		// 1. 处理固定法术穿透
		TargetMagicResistance = FMath::Max(0.0f, TargetMagicResistance - MagicPenetration);
		// 2. 处理百分比法术穿透
		TargetMagicResistance = FMath::Max(0.0f, TargetMagicResistance * (1.0f - FMath::Min(MagicPenetrationPercent, 100.0f) / 100.0f));
		// 3. 计算法抗减免（计算出来的是免伤率）
		float MagicResistanceReduction = TargetMagicResistance / (TargetMagicResistance + 100.0f);
		BaseMagicDamage *= (1.0f - FMath::Min(MagicResistanceReduction / 100.0f + DamageReduction/100.0f, 1.0f));
		// 4. 应用伤害加深（百分比提升）
		BaseMagicDamage *= (1.0f + DamageAmp / 100.0f);
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
			UCAttributeSet::GetMagicDamageAttribute(), //获取到伤害属性
			EGameplayModOp::Override, 
			BaseMagicDamage	//伤害
			));
	}
	
	// 计算基础真实伤害值
	float BaseTrueDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BaseTrueDamageDef, 
		EvaluateParameters, 
		BaseTrueDamage
	);
	if (BaseTrueDamage > 0.0f)
	{
		// 计算伤害减免
		BaseTrueDamage *= (1.0f - FMath::Min(DamageReduction/100.0f, 1.0f));
		// 应用伤害加深（百分比提升）
		BaseTrueDamage *= (1.0f + DamageAmp / 100.0f);
		
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
			UCAttributeSet::GetTrueDamageAttribute(), //获取到伤害属性
			EGameplayModOp::Override, 
			BaseTrueDamage	//伤害
			));
	}
}
