// 幻雨喜欢小猫咪


#include "ListDataObject_Value.h"

void UListDataObject_Value::SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter)
{
	// 将传入的动态获取器赋值给内部成员变量
	DataDynamicGetter = InDynamicGetter;
}

void UListDataObject_Value::SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter)
{
	// 将传入的动态设置器赋值给内部成员变量
	DataDynamicSetter = InDynamicSetter;
}
