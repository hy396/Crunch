// 幻雨喜欢小猫咪


#include "OptionsDataInteractionHelper.h"

#include "FrontendUI/FrontendSettings/FrontendGameUserSettings.h"

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath)
	: CachedDynamicFunctionPath(InSetterOrGetterFuncPath) //在构造函数体执行之前，直接使用参数InSetterOrGetterFuncPath初始化类成员变量CachedDynamicFunctionPath。
{
	// 获取游戏用户设置单例
	CachedWeakGameUserSettings = UFrontendGameUserSettings::Get();
}

FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	FString OutStringValue;
	PropertyPathHelpers::GetPropertyValueAsString(
		CachedWeakGameUserSettings.Get(),    // 获取原对象
		CachedDynamicFunctionPath,           // 函数/属性路径
		OutStringValue                        // 输出字符串
	);
	return OutStringValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InValueString)
{
	// 设置属性值
	PropertyPathHelpers::SetPropertyValueFromString(CachedWeakGameUserSettings.Get(), CachedDynamicFunctionPath, InValueString);
}
