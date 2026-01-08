// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonNumericTextBlock.h"
//OptionsDataInteractionHelper
#include "FrontendUI/Widgets/Options/OptionsDataInteractionHelper.h"
#include "ListDataObject_Value.h"
#include "ListDataObject_Scalar.generated.h"

/**
 * 标量列表数据对象类
 * 用于处理标量数值类型的选项数据，支持范围、步长等配置
 */
UCLASS()
class CRUNCH_API UListDataObject_Scalar : public UListDataObject_Value
{
	GENERATED_BODY()
public:
	// 为各种属性生成访问器方法
	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange)
	LIST_DATA_ACCESSOR(TRange<float>, OutputValueRange)
	LIST_DATA_ACCESSOR(float, SliderStepSize)
	LIST_DATA_ACCESSOR(ECommonNumericType, DisplayNumericType)
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, NumberedFormattingOptions)

	// 创建无小数位的数字格式化选项
	static FCommonNumberFormattingOptions NoDecimal();
	
	// 创建指定小数位数的数字格式化选项
	static FCommonNumberFormattingOptions WithDecimal(int32 NumFracDigit);

	// 获取当前值
	float GetCurrentValue() const;

	// 通过滑块设置当前值
	void SetCurrentValueFromSlider(float InNewValue);
	
private:
	// 显示值范围
	TRange<float> DisplayValueRange = TRange<float>(0.f, 1.f);
	
	// 输出值范围
	TRange<float> OutputValueRange = TRange<float>(0.f, 1.f);
	
	// 滑块步长大小
	float SliderStepSize = 0.f;
	
	// 显示数字类型
	ECommonNumericType DisplayNumericType = ECommonNumericType::Number;
	
	// 数字格式化选项
	FCommonNumberFormattingOptions NumberedFormattingOptions;

	// 将字符串转换为浮点数
	float StringToFloat(const FString& InString) const;

	
	// begin UListDataObject_Base 接口
	// 检查是否可以重置回默认值
	virtual bool CanResetBackToDefaultValue() const override;
	
	// 尝试重置回默认值
	virtual bool TryResetBackToDefaultValue() override;
	
	// 当依赖数据修改时的处理
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;
	// end UListDataObject_Base 接口
};
