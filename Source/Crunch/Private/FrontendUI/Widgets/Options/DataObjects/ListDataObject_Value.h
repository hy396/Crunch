// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "ListDataObject_Value.generated.h"

class FOptionsDataInteractionHelper;

/**
 * 值类型列表数据对象基类
 * 抽象类，用于处理具有具体值的选项数据对象
 */
UCLASS(Abstract)
class CRUNCH_API UListDataObject_Value : public UListDataObject_Base
{
	GENERATED_BODY()
public:
	// 设置数据动态获取器
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter);
	
	// 设置数据动态设置器
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter);

	// 设置默认值字符串
	void SetDefaultValueFromString(const FString& InDefaultValue){DefaultStringValue = InDefaultValue;};

	// begin UListDataObject_Base 接口
	// 检查是否有默认值
	virtual bool HasDefaultValue() const override{return DefaultStringValue.IsSet();}
	// end UListDataObject_Base 接口
	
protected:
	// 数据动态获取器
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;
	
	// 数据动态设置器
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;

	// 获取默认值字符串
	FString GetDefaultValueAsString()const{return DefaultStringValue.GetValue();}

	
private:
	// 可选的默认字符串值
	TOptional<FString> DefaultStringValue;  // TOptional的好处是有函数isSet检测是否被设置过  GetValue获取该值
};
