// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Value.h"
#include "ListDataObject_String.generated.h"

/**
 * 字符串列表数据对象类
 * 用于处理字符串类型的选项数据，支持动态选项添加和选择
 */
UCLASS()
class CRUNCH_API UListDataObject_String : public UListDataObject_Value
{
	GENERATED_BODY()
public:
	// 添加动态选项
	void AddDynamicOption(const FString& InStringValue, const FText& InDisplayText);

	// 获取可用选项文本数组
	FORCEINLINE const TArray<FText>& GetAvailableOptionsTextArray() const { return AvailableOptionsTextArray; }
	
	// 获取当前显示文本
	FORCEINLINE FText GetCurrentDisplayText() const { return CurrentDisplayText; }

	// 切换到下一个选项
	void AdvanceToNextOption();
	
	// 切换到上一个选项
	void BackToPreviousOption();

	// 当旋转器初始化值更改时调用
	void OnRotatorInitiatedValueChange(const FText& InNewSelectedText);
	
protected:

	// begin UListDataObject_Base 接口
	// 数据对象初始化时调用
	virtual void OnDataObjectInitialized() override;
	
	// 检查是否可以重置回默认值
	virtual bool CanResetBackToDefaultValue() const override;
	
	// 尝试重置回默认值
	virtual bool TryResetBackToDefaultValue() override;

	// 检查是否可以设置为强制字符串值
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const override;
	
	// 当设置为强制字符串值时调用
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) override;
	// end UListDataObject_Base 接口
	
	// 尝试根据字符串值设置显示文本
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);
	
	// 当前字符串值
	FString CurrentStringValue;
	
	// 当前显示文本
	FText CurrentDisplayText;
	
	// 可用选项字符串数组
	TArray<FString> AvailableOptionsStringArray;
	
	// 可用选项文本数组
	TArray<FText> AvailableOptionsTextArray;
};

/**
 * 字符串布尔型列表数据对象类
 * 专门用于处理布尔型选项的特殊字符串列表数据对象
 */
UCLASS()
class CRUNCH_API UListDataObject_StringBool : public UListDataObject_String
{
	GENERATED_BODY()
public:
	// 覆盖真值显示文本
	void OverrideTrueDisplayText(const FText& InNewTrueDisplayText);
	
	// 覆盖假值显示文本
	void OverrideFalseDisplayText(const FText& InNewFalseDisplayText);

	// 设置真值为默认值
	void SetTrueAsDefaultValue();
	
	// 设置假值为默认值
	void SetFalseAsDefaultValue();
	
protected:

	// begin UListDataObject_String 接口
	// 数据对象初始化时调用
	virtual void OnDataObjectInitialized() override;
	// end UListDataObject_String 接口
	
private:
	// 尝试初始化布尔值
	void TryInitBoolValues();
	
	// 真值字符串常量
	const FString TrueString = TEXT("True");
	
	// 假值字符串常量
	const FString FalseString = TEXT("false");
};

/**
 * 字符串枚举型列表数据对象类
 * 专门用于处理枚举类型选项的特殊字符串列表数据对象
 */
UCLASS()
class CRUNCH_API UListDataObject_StringEnum : public UListDataObject_String
{
	GENERATED_BODY()
public:
	// 将枚举值转换为对应的字符串名称
	template<typename EnumType>
	void AddEnumOption(EnumType InEnumOption, const FText& InDisplayText)  // 枚举值 + 显示文本
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();  //获取枚举类型的UEnum元数据
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption); // 将枚举值转为字符串

		AddDynamicOption(ConvertedEnumString, InDisplayText);	
	}

	template<typename EnumType>
	EnumType GetCurrentValueAsEnum() const
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		return static_cast<EnumType>(StaticEnumOption->GetValueByNameString(CurrentStringValue));
	}

	template<typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumOption)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();  //获取枚举类型的UEnum元数据
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption); // 将枚举值转为字符串
		SetDefaultValueFromString(ConvertedEnumString);
	}
};

/**
 * 字符串整型列表数据对象类
 * 专门用于处理整数类型选项的特殊字符串列表数据对象
 */
UCLASS()
class CRUNCH_API UListDataObject_StringInteger : public UListDataObject_String
{
	GENERATED_BODY()
public:
	// 添加整数选项
	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);
	
protected:
	// begin UListDataObject_String 接口
	// 数据对象初始化时调用
	virtual void OnDataObjectInitialized() override;
	// end UListDataObject_String 接口

	// 当依赖数据修改时调用
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;

private:
};