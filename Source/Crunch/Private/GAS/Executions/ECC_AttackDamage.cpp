// 幻雨喜欢小猫咪


#include "GAS/Executions/ECC_AttackDamage.h"

#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CHeroAttributeSet.h"
struct FSourceDamageStatics
{
	// 最大生命值
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	// 当前生命值
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	// 最大魔法值
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxMana);
	// 当前魔法值
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mana);
	// 攻击力
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	// 魔法强度
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPower);
	// 自己护甲
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	// 自己魔抗
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
	// 移动速度
	DECLARE_ATTRIBUTE_CAPTUREDEF(MoveSpeed);

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

	FSourceDamageStatics()
	{
		// 参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MaxHealth, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, Health, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MaxMana, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, Mana, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MagicPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, Armor, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MagicResistance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MoveSpeed, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, ArmorPenetrationPercent, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, MagicPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, MagicPenetrationPercent, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, DamageAmplification, Source, false);
	}
};

// 静态数据访问函数（单例模式）
static FSourceDamageStatics& SourceDamageStatics()
{
	static FSourceDamageStatics Statics;
	return Statics;
}
struct FTargetDamageStatics
{
	// 敌方的物理防御
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	// 敌方的法术抗性
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
	// 伤害减免
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);

	FTargetDamageStatics()
	{
	 	// 参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCAttributeSet, MagicResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCHeroAttributeSet, DamageReduction, Target, false);
	 }
};


// 静态数据访问函数（单例模式）
static FTargetDamageStatics& TargetDamageStatics()
{
	static FTargetDamageStatics Statics;
	return Statics;
}

UECC_AttackDamage::UECC_AttackDamage()
{
	// 将属性添加到捕获列表中
	// 添加源
	RelevantAttributesToCapture.Add(SourceDamageStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MaxManaDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().ManaDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MagicPowerDef);
	
	RelevantAttributesToCapture.Add(SourceDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MagicResistanceDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MoveSpeedDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().ArmorPenetrationPercentDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MagicPenetrationDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().MagicPenetrationPercentDef);
	RelevantAttributesToCapture.Add(SourceDamageStatics().DamageAmplificationDef);

	// 添加目标
	RelevantAttributesToCapture.Add(TargetDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(TargetDamageStatics().MagicResistanceDef);
	RelevantAttributesToCapture.Add(TargetDamageStatics().DamageReductionDef);
}

void UECC_AttackDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
//#if WITH_SERVER_CODE
	//存储标签和属性快照对应的Map
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	// TODO:添加新的需要修改的属性值，添加新的标签和值
	//添加标签和属性快照对应的数据
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_MaxHealth, SourceDamageStatics().MaxHealthDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_Health, SourceDamageStatics().HealthDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_MaxMana, SourceDamageStatics().MaxManaDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_Mana, SourceDamageStatics().ManaDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_AttackPower, SourceDamageStatics().AttackPowerDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_MagicPower, SourceDamageStatics().MagicPowerDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_Armor, SourceDamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_MagicResistance, SourceDamageStatics().MagicResistanceDef);
	TagsToCaptureDefs.Add(TGameplayTags::Attribute_MoveSpeed, SourceDamageStatics().MoveSpeedDef);
	
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

	float DamageAdd = 0.0f;
	for (auto& TagToCaptureDef : TagsToCaptureDefs)
	{
		const float Coefficient = Spec.GetSetByCallerMagnitude(TagToCaptureDef.Key, false, -1);
		if (Coefficient <= 0.0f) continue;
		float AttributeValue = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagToCaptureDef.Value, EvaluateParameters, AttributeValue);
		DamageAdd += AttributeValue * Coefficient / 100.0f;
	}
	
	// 获取伤害加深
	float DamageAmp = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().DamageAmplificationDef, EvaluateParameters, DamageAmp);
	// 获取敌方的伤害减免
	float DamageReduction = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().DamageReductionDef, EvaluateParameters, DamageReduction);

	// 获取基础攻击伤害
	float BaseAttackDamage = Spec.GetSetByCallerMagnitude(TGameplayTags::DamageType_AttackDamage, false, -1);
	// 物理伤害的处理
	if (BaseAttackDamage > 0.0f)
	{
		BaseAttackDamage += DamageAdd;
		// 获取护甲穿透百分比
		float ArmorPenetrationPercent = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().ArmorPenetrationPercentDef, EvaluateParameters, ArmorPenetrationPercent);
		// 获取护甲穿透
		float ArmorPenetration = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().ArmorPenetrationDef, EvaluateParameters, ArmorPenetration);
		// 获取目标护甲
		float TargetArmor = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
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
			
			// // 吸血效果实现
			// float LifeStealPercent = Spec.GetSetByCallerMagnitude(TGameplayTags::Attribute_LifeSteal, false, 0.0f);
			// if (LifeStealPercent > 0.0f)
			// {
			// 	float LifeStealAmount = BaseAttackDamage * LifeStealPercent / 100.0f;
				
			// 	// 手动获取Source的ASC并应用治疗效果
			// 	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
			// 	if (SourceASC)
			// 	{
			// 		// 创建临时GE来治疗Source
			// 		UGameplayEffect* LifeStealEffect = NewObject<UGameplayEffect>(GetTransientPackage(), TEXT("LifeStealEffect"));
			// 		LifeStealEffect->DurationPolicy = EGameplayEffectDurationType::Instant;
					
			// 		// 添加治疗效果
			// 		FGameplayModifierInfo HealModifier;
			// 		HealModifier.Attribute = UCAttributeSet::GetHealthAttribute();
			// 		HealModifier.ModifierOp = EGameplayModOp::Additive;
			// 		HealModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(LifeStealAmount));
			// 		LifeStealEffect->Modifiers.Add(HealModifier);
					
			// 		// 应用效果到Source
			// 		SourceASC->ApplyGameplayEffectToSelf(LifeStealEffect, 1.0f, SourceASC->MakeEffectContext());
			// 	}
			// }
		}
	}

	// 计算基础法术伤害值
	// float BaseMagicDamage = 0.0f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
	// 	DamageStatics().BaseMagicDamageDef, 
	// 	EvaluateParameters, 
	// 	BaseMagicDamage
	// );
	float BaseMagicDamage = Spec.GetSetByCallerMagnitude(TGameplayTags::DamageType_MagicDamage, false, -1.f);
	if (BaseMagicDamage > 0)
	{
		BaseMagicDamage += DamageAdd;
		// 获取法术穿透百分比
		float MagicPenetrationPercent = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			SourceDamageStatics().MagicPenetrationPercentDef,
			EvaluateParameters, MagicPenetrationPercent);
		// 获取法术穿透
		float MagicPenetration = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			SourceDamageStatics().MagicPenetrationDef,
			EvaluateParameters, MagicPenetration);
		// 获取目标法抗
		float TargetMagicResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			TargetDamageStatics().MagicResistanceDef, EvaluateParameters, TargetMagicResistance);
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
	// float BaseTrueDamage = 0.0f;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
	// 	DamageStatics().BaseTrueDamageDef, 
	// 	EvaluateParameters, 
	// 	BaseTrueDamage
	// );
	float BaseTrueDamage = Spec.GetSetByCallerMagnitude(TGameplayTags::DamageType_TrueDamage, false, -1.f);
	if (BaseTrueDamage > 0.0f)
	{
		BaseTrueDamage += DamageAdd;
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
//#endif // #if WITH_SERVER_CODE
}
