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

// 静态Tag→CaptureDef映射，程序启动时只构建一次（避免每次伤害计算都重建TMap）
static const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& GetTagsToCaptureDefs()
{
	static TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> Map = []()
	{
		TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> M;
		M.Add(TGameplayTags::Attribute_MaxHealth, SourceDamageStatics().MaxHealthDef);
		M.Add(TGameplayTags::Attribute_Health, SourceDamageStatics().HealthDef);
		M.Add(TGameplayTags::Attribute_MaxMana, SourceDamageStatics().MaxManaDef);
		M.Add(TGameplayTags::Attribute_Mana, SourceDamageStatics().ManaDef);
		M.Add(TGameplayTags::Attribute_AttackPower, SourceDamageStatics().AttackPowerDef);
		M.Add(TGameplayTags::Attribute_MagicPower, SourceDamageStatics().MagicPowerDef);
		M.Add(TGameplayTags::Attribute_Armor, SourceDamageStatics().ArmorDef);
		M.Add(TGameplayTags::Attribute_MagicResistance, SourceDamageStatics().MagicResistanceDef);
		M.Add(TGameplayTags::Attribute_MoveSpeed, SourceDamageStatics().MoveSpeedDef);
		return M;
	}();
	return Map;
}

// 穿透计算辅助函数（物理/魔法共用，避免重复代码）
static float ApplyPenetration(float Defense, float FlatPenetration, float PercentPenetration)
{
	// 1. 固定穿透
	Defense = FMath::Max(0.0f, Defense - FlatPenetration);
	// 2. 百分比穿透
	Defense = FMath::Max(0.0f, Defense * (1.0f - FMath::Min(PercentPenetration, 100.0f) / 100.0f));
	return Defense;
}

// 最终伤害减免计算（防御减免 + 通用伤害减免）
static float ApplyDamageReduction(float BaseDamage, float DefenseReduction, float DamageReduction, float DamageAmp)
{
	BaseDamage *= (1.0f - FMath::Min(DefenseReduction / 100.0f + DamageReduction / 100.0f, 1.0f));
	BaseDamage *= (1.0f + DamageAmp / 100.0f);
	return BaseDamage;
}

void UECC_AttackDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 使用静态缓存的TMap（不再每次调用都重建）
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToCaptureDefs = GetTagsToCaptureDefs();
	
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
		// 获取穿透属性
		float ArmorPenetrationPercent = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().ArmorPenetrationPercentDef, EvaluateParameters, ArmorPenetrationPercent);
		float ArmorPenetration = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().ArmorPenetrationDef, EvaluateParameters, ArmorPenetration);
		float TargetArmor = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
		// 穿透计算（复用helper）
		TargetArmor = ApplyPenetration(TargetArmor, ArmorPenetration, ArmorPenetrationPercent);
		float ArmorReduction = TargetArmor / (TargetArmor + 100.0f);
		// 减免 + 伤害加深（复用helper）
		BaseAttackDamage = ApplyDamageReduction(BaseAttackDamage, ArmorReduction, DamageReduction, DamageAmp);
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
		// 获取穿透属性
		float MagicPenetrationPercent = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().MagicPenetrationPercentDef, EvaluateParameters, MagicPenetrationPercent);
		float MagicPenetration = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(SourceDamageStatics().MagicPenetrationDef, EvaluateParameters, MagicPenetration);
		float TargetMagicResistance = 0.0f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetDamageStatics().MagicResistanceDef, EvaluateParameters, TargetMagicResistance);
		// 穿透计算（复用helper）
		TargetMagicResistance = ApplyPenetration(TargetMagicResistance, MagicPenetration, MagicPenetrationPercent);
		float MagicResistanceReduction = TargetMagicResistance / (TargetMagicResistance + 100.0f);
		// 减免 + 伤害加深（复用helper）
		BaseMagicDamage = ApplyDamageReduction(BaseMagicDamage, MagicResistanceReduction, DamageReduction, DamageAmp);
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
		// 真伤无防御减免，只受通用伤害减免影响（复用helper，DefenseReduction=0）
		BaseTrueDamage = ApplyDamageReduction(BaseTrueDamage, 0.0f, DamageReduction, DamageAmp);
		
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
			UCAttributeSet::GetTrueDamageAttribute(), //获取到伤害属性
			EGameplayModOp::Override, 
			BaseTrueDamage	//伤害
			));
	}
//#endif // #if WITH_SERVER_CODE
}
