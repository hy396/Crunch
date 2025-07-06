// 幻雨喜欢小猫咪


#include "UI/Gameplay/ValueGauge.h"

#include "AbilitySystemComponent.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	// 设置进度条颜色
	ProgressBar->SetFillColorAndOpacity(BarColor);
	if (ValueText)
	{
		// 获取当前字体设置
		FSlateFontInfo FontInfo = ValueText->GetFont();
		// 更新字体大小
		FontInfo.Size = TextSize;
		// 应用新的字体设置到文本组件
		ValueText->SetFont(FontInfo);
	}
}

void UValueGauge::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,
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
			SetValue(Value, MaxValue);
		}
		// 注册属性变化回调，当属性值发生变化时更新数值指示器显示
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::ValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UValueGauge::MaxValueChanged);
	}
}

void UValueGauge::SetValue(const float NewValue, const float NewMaxValue)
{
	// 缓存属性值
	CachedValue = NewValue;
	CachedMaxValue = NewMaxValue;
	
	if (NewMaxValue == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value Gauge: %s, 新的最大值不能为0"), *GetName());
		return;
	}
	// 设置进度条百分比
	ProgressBar->SetPercent(NewValue / NewMaxValue);

	// 设置数字格式选项，最大小数位数为0
	const FNumberFormattingOptions FormatOps = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	// 更新文本显示
	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),			 // 格式字符串
			FText::AsNumber(NewValue, &FormatOps),       // 当前值
			FText::AsNumber(NewMaxValue, &FormatOps)     // 最大值
		)
	);
}

void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(ChangeData.NewValue, CachedMaxValue);
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangeData)
{
	SetValue(CachedValue, ChangeData.NewValue);
}
