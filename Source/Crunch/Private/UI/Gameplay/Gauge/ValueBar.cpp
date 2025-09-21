// 幻雨喜欢小猫咪


#include "ValueBar.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "CommonNumericTextBlock.h"

void UValueBar::NativeConstruct()
{
	Super::NativeConstruct();
	SetDynamicMaterials();
}
void UValueBar::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,
												 const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	if (AbilitySystemComponent)
	{
		// 从能力系统组件中获取当前属性值和最大值属性值
		bool bFound;
		float Value = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bFound);
		float MaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute, bFound);

		// 如果成功找到对应的属性值，则更新数值指示器的显示
		if (bFound)
		{
			// 更新数值指示器的显示
			CurrentValueNumber->InterpolateToValue(Value, 1.0f, 4.0f, 0.f);
			MaxValueNumber->InterpolateToValue(MaxValue, 1.0f, 4.0f, 0.f);
			SetValue(Value, MaxValue);
		}
		// 注册属性变化回调，当属性值发生变化时更新数值指示器显示
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueBar::ValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UValueBar::MaxValueChanged);
		InitializeBarVisuals();
	}
}
void UValueBar::SetDynamicMaterials()
{
	BarBorderMID = BarBorder->GetDynamicMaterial();
	BarFillMID = BarFill->GetDynamicMaterial();
	BarGlowMID = BarGlow->GetDynamicMaterial();
}

void UValueBar::InitializeBarVisuals()
{
	BarBorderMID->SetScalarParameterValue("Health_Current", 1.f);
	BarFillMID->SetScalarParameterValue("Health_Current", 1.f);
	BarGlowMID->SetScalarParameterValue("Health_Current", 1.f);

	BarBorderMID->SetScalarParameterValue("Health_Updated", 1.f);
	BarFillMID->SetScalarParameterValue("Health_Updated", 1.f);
	BarGlowMID->SetScalarParameterValue("Health_Updated", 1.f);

	// CurrentValueNumber->InterpolateToValue(NormalizedHealth * 100.f, 1.0f, 4.0f, 0.f);

	ResetAnimatedState();
}

void UValueBar::ResetAnimatedState()
{
	BarBorderMID->SetScalarParameterValue("Animate_Damage", 0.f);
	BarFillMID->SetScalarParameterValue("Animate_Damage", 0.f);
	BarGlowMID->SetScalarParameterValue("Animate_Damage", 0.f);

	BarBorderMID->SetScalarParameterValue("Animate_DamageFade", 0.f);
	BarFillMID->SetScalarParameterValue("Animate_DamageFade", 0.f);
	BarGlowMID->SetScalarParameterValue("Animate_DamageFade", 0.f);

	BarBorderMID->SetScalarParameterValue("Animate_Glow_AlphaChange", 0.f);
	BarGlowMID->SetScalarParameterValue("Animate_Glow_AlphaChange", 0.f);

	BarBorderMID->SetScalarParameterValue("Animate_Glow_ColorChange", 0.f);
	BarGlowMID->SetScalarParameterValue("Animate_Glow_ColorChange", 0.f);
}


void UValueBar::ValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(ChangeData.NewValue, CachedMaxValue);
}

void UValueBar::MaxValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(CachedValue, ChangeData.NewValue);
}

void UValueBar::SetValue(const float New_Value, const float NewMaxValue)
{
	OldValue = CachedValue;
	NewValue = New_Value;
	if (OldValue == 0 && NewValue == NewMaxValue)
	{
		PlayAnimationForward(OnSpawned);
	}
	// 缓存属性值
	CachedValue = New_Value;
	CachedMaxValue = NewMaxValue;
	if (NewMaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value Gauge: %s, 新的最大值不能为0"), *GetName());
		return;
	}
	NormalizedHealth = NewValue / NewMaxValue;

	BarBorderMID->SetScalarParameterValue("Health_Current", OldValue / CachedMaxValue);
	BarFillMID->SetScalarParameterValue("Health_Current", OldValue / CachedMaxValue);
	BarGlowMID->SetScalarParameterValue("Health_Current", OldValue / CachedMaxValue);
	
	BarBorderMID->SetScalarParameterValue("Health_Updated", NewValue / CachedMaxValue);
	BarFillMID->SetScalarParameterValue("Health_Updated", NewValue / CachedMaxValue);
	BarGlowMID->SetScalarParameterValue("Health_Updated", NewValue / CachedMaxValue);

	ResetAnimatedState();
	if (MaxValueNumber->GetTargetValue() != CachedMaxValue)
	{
		// 更新最大值
		MaxValueNumber->InterpolateToValue(CachedMaxValue, 1.0f, 4.0f, 0.f);
	}

	if (!(CurrentValueNumber->GetTargetValue() == NewValue) && !(NewValue == OldValue))
	{
		BarBorderMID->SetScalarParameterValue("DamageOrHealing", 0.f);
		BarFillMID->SetScalarParameterValue("DamageOrHealing", 0.f);
		BarGlowMID->SetScalarParameterValue("DamageOrHealing", 0.f);
		if (NewValue >= OldValue)
		{
			PlayAnimationForward(OnHealed);
		}else
		{
			PlayAnimationForward(OnDamaged);
		}
		CurrentValueNumber->SetCurrentValue(OldValue);
		CurrentValueNumber->InterpolateToValue(NewValue, 1.0f, 4.0f, 0.f);
		// 我可能还会给蓝条使用，因此忽略掉这个
		if (NewValue == OldValue)
		{
			// BindToAnimationFinished(OnDamaged, UK2Node_CreateDelegate[]()
			// {
			// 	ResetAnimatedState();
			// })
		}
	}
}

void UValueBar::SetRegenValueTextToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayAttribute& Attribute)
{
	if (AbilitySystemComponent)
	{
		// 从能力系统组件中获取当前属性值和最大值属性值
		bool bFound;
		float Value = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bFound);
		// 如果成功找到对应的属性值，则更新数值指示器的显示
		if (bFound)
		{
			SetRegenValue(Value);
		}
		// 注册属性变化回调，当属性值发生变化时更新数值指示器显示
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueBar::RegenValueChanged);
	}
}

void UValueBar::SetRegenValue(float NewRegenValue)
{
	// 设置数字格式选项，最大小数位数为0
	const FNumberFormattingOptions FormatOps = FNumberFormattingOptions().SetMaximumFractionalDigits(2);
	// 更新文本显示
	RegenValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/s"),			 // 格式字符串
			FText::AsNumber(NewRegenValue, &FormatOps)      // 当前值
		)
	);
}

void UValueBar::RegenValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetRegenValue(ChangeData.NewValue);
}