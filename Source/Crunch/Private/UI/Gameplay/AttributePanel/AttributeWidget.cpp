// 幻雨喜欢小猫咪


#include "AttributeWidget.h"
#include "AbilitySystemComponent.h"

bool UAttributeWidget::BindForAscToAttribute(UAbilitySystemComponent* AbilitySystemComponent, const FText& AttributeName,
                                             const FGameplayAttribute& Attribute)
{
	bool bFound = false;
	if (AbilitySystemComponent)
	{
		float NewAttributeValue = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bFound);
		if (bFound)
		{
			// 创建一个FOnAttributeChangeData结构体来传递属性值
			FOnAttributeChangeData ChangeData;
			ChangeData.NewValue = NewAttributeValue;
			
			SetAttributeValue(ChangeData);
			Hy_AttributeName->SetText(AttributeName);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAttributeWidget::SetAttributeValue);
		}
	}
	return bFound;
}

void UAttributeWidget::SetAttributeValue(const FOnAttributeChangeData& ChangeData)
{
	AttributeValue->InterpolateToValue(ChangeData.NewValue, 1.0f, 4.0f, 0.f);
}
