// 幻雨喜欢小猫咪


#include "GAS/Executions/ECC_AttackDamage.h"

#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"

struct FDamageStatics
{
	// FGameplayEffectAttributeCaptureDefinition
	// 基础伤害
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseDamage);
	// 攻击百分比
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPowerCoefficient);
	// 物理攻击
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	// 护甲穿透
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	// 护甲穿透百分比
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationPercent);
	// 伤害加深
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageAmplification);
	// 敌方的防御
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	// 伤害减免
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);

	FDamageStatics()
	{
	 	// 参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, BaseDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, AttackPowerCoefficient, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, ArmorPenetrationPercent, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, DamageAmplification, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, Armor, Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerCoefficientDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationPercentDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageAmplificationDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
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

	// 计算基础伤害值
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BaseDamageDef, 
		EvaluateParameters, 
		BaseDamage
	);

	// 获取攻击计算系数
	float AttackPowerCoefficient = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerCoefficientDef, EvaluateParameters, AttackPowerCoefficient);

	// 获取攻击力
	float AttackPower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluateParameters, AttackPower);

	// 获取护甲穿透百分比
	float ArmorPenetrationPercent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationPercentDef, EvaluateParameters, ArmorPenetrationPercent);

	// 获取护甲穿透
	float ArmorPenetration = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluateParameters, ArmorPenetration);

	// 获取目标护甲
	float TargetArmor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);

	// 获取伤害加深
	float DamageAmp = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageAmplificationDef, EvaluateParameters, DamageAmp);

	// 获取敌方的伤害减免
	float DamageReduction = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef, EvaluateParameters, DamageReduction);

	// 1. 计算基础伤害
	// 公式: 基础伤害 + 攻击力 × 攻击力系数
	float PhysicalDamage = BaseDamage + AttackPower * (AttackPowerCoefficient / 100.0f);
	
	// 2. 处理固定护甲穿透
	TargetArmor = FMath::Max(0.0f, TargetArmor - ArmorPenetration);
	
	// 3. 处理百分比护甲穿透
	TargetArmor = FMath::Max(0.0f, TargetArmor * (1.0f - FMath::Min(ArmorPenetrationPercent, 100.0f) / 100.0f));
	
	// 4. 计算护甲减免（计算出来的是免伤率）
	float ArmorReduction = TargetArmor / (TargetArmor + 100.0f);
	PhysicalDamage *= (1.0f - FMath::Min(ArmorReduction + DamageReduction/100.0f, 1.0f));

	// 5. 应用伤害加深（百分比提升）
	PhysicalDamage *= (1.0f + DamageAmp / 100.0f);
	//DamageReduction 敌方的伤害减免
	// 6. 应用固定伤害减免
	// float FinalDamage = FMath::Max(0.0f, PhysicalDamage - DamageReduction);

	// 7. 输出到AttackDamage属性
	if (PhysicalDamage > 0.0f)
	{
		// 添加输出修饰符
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
			UCAttributeSet::GetAttackDamageAttribute(), //获取到伤害属性
			EGameplayModOp::Additive, //加法
			PhysicalDamage	//伤害
			));
	}
}
