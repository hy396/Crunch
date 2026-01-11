// 幻雨喜欢小猫咪


#include "ListDataObject_Scalar.h"

FCommonNumberFormattingOptions UListDataObject_Scalar::NoDecimal()
{
	// 创建数字格式化选项对象
	FCommonNumberFormattingOptions Options;
	
	// 设置最大小数位数为0
	Options.MaximumFractionalDigits = 0;
	
	// 返回配置好的选项
	return Options;
}

FCommonNumberFormattingOptions UListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	// 创建数字格式化选项对象
	FCommonNumberFormattingOptions Options;
	
	// 设置最大小数位数为指定值
	Options.MaximumFractionalDigits = NumFracDigit;
	
	// 返回配置好的选项
	return Options;
}

float UListDataObject_Scalar::GetCurrentValue() const
{
	// 检查数据动态获取器是否存在
	if (DataDynamicGetter)
	{
		// 将输出值范围映射到显示值范围，并返回转换后的当前值
		return FMath::GetMappedRangeValueClamped(
			OutputValueRange,
			DisplayValueRange,
			StringToFloat(DataDynamicGetter->GetValueAsString()));
	}
	
	// 如果没有数据动态获取器，则返回默认值0.0f
	return 0.0f;
}

void UListDataObject_Scalar::SetCurrentValueFromSlider(float InNewValue)
{
	// 检查数据动态设置器是否存在
	if (DataDynamicSetter)
	{
		// 将显示值范围映射到输出值范围，并对新值进行限制
		const float ClampedValue = FMath::GetMappedRangeValueClamped(
			DisplayValueRange,
			OutputValueRange,
			InNewValue);
			
		// 将限制后的值转换为字符串并设置
		DataDynamicSetter->SetValueFromString(LexToString(ClampedValue));

		// 通知列表数据已修改
		NotifyListDataModified(this);
	}
}

float UListDataObject_Scalar::StringToFloat(const FString& InString) const
{
	// 定义转换后的值变量
	float OutConvertedValue = 0.f;
	
	// 将字符串转换为浮点数
	LexFromString(OutConvertedValue,*InString);
	
	// 返回转换后的值
	return OutConvertedValue;
}

bool UListDataObject_Scalar::CanResetBackToDefaultValue() const
{
	// 检查是否有默认值且数据动态获取器存在
	if (HasDefaultValue() && DataDynamicGetter)
	{
		// 获取默认值并转换为浮点数
		const float DefaultValue = StringToFloat(GetDefaultValueAsString());
		
		// 获取当前值并转换为浮点数
		const float CurrentValue = StringToFloat(DataDynamicGetter->GetValueAsString());
		
		// 比较默认值和当前值是否近似相等（精度为0.01f）
		return !FMath::IsNearlyEqual(DefaultValue, CurrentValue, 0.01f);
	}
	
	// 如果没有默认值或数据动态获取器不存在，则返回false
	return false;
}

bool UListDataObject_Scalar::TryResetBackToDefaultValue()
{
	// 检查是否可以重置回默认值
	if (CanResetBackToDefaultValue())
	{
		// 检查数据动态设置器是否存在
		if (DataDynamicSetter)
		{
			// 使用默认值字符串设置当前值
			DataDynamicSetter->SetValueFromString(GetDefaultValueAsString());
			
			// 通知列表数据已修改，原因是重置为默认值
			NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
			
			// 返回成功
			return true;
		}
	}
	
	// 返回失败
	return false;
}

void UListDataObject_Scalar::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	// 通知列表数据已修改，原因是依赖数据修改
	NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	
	// 调用父类的实现
	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}
