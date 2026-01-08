// 幻雨喜欢小猫咪


#include "ListDataObject_String.h"

#include "FrontendUI/Widgets/Options/OptionsDataInteractionHelper.h"

void UListDataObject_String::AddDynamicOption(const FString& InStringValue, const FText& InDisplayText)
{
	// 将字符串值添加到可用选项字符串数组
	AvailableOptionsStringArray.Add(InStringValue);
	
	// 将显示文本添加到可用选项文本数组
	AvailableOptionsTextArray.Add(InDisplayText);
}

void UListDataObject_String::AdvanceToNextOption()
{
	// 如果可用选项数组为空，则直接返回
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}
	
	// 获取当前显示索引
	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	
	// 计算下一个显示索引
	const int32 NextDisplayIndex = CurrentDisplayIndex + 1;
	
	// 检测下一个索引是否有效
	const bool bIsNextIndexValid = AvailableOptionsStringArray.IsValidIndex(NextDisplayIndex);
	
	// 如果下一个索引有效，则使用下一个选项，否则回到第一个选项
	if (bIsNextIndexValid)
	{
		CurrentStringValue = AvailableOptionsStringArray[NextDisplayIndex];
	}else
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	// 尝试根据字符串值设置显示文本
	TrySetDisplayTextFromStringValue(CurrentStringValue);

	// 如果存在数据动态设置器，则更新值并通知修改
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this);
	}
}

void UListDataObject_String::BackToPreviousOption()
{
	// 如果可用选项数组为空，则直接返回
	if (AvailableOptionsStringArray.IsEmpty() || AvailableOptionsTextArray.IsEmpty())
	{
		return;
	}
	
	// 获取当前显示索引
	const int32 CurrentDisplayIndex = AvailableOptionsStringArray.IndexOfByKey(CurrentStringValue);
	
	// 计算上一个显示索引
	const int32 PreviousDisplayIndex = CurrentDisplayIndex - 1;

	// 如果上一个索引有效，则使用上一个选项，否则使用最后一个选项
	if (AvailableOptionsStringArray.IsValidIndex(PreviousDisplayIndex))
	{
		CurrentStringValue = AvailableOptionsStringArray[PreviousDisplayIndex];
	}else
	{
		CurrentStringValue = AvailableOptionsStringArray.Last();
	}
	
	// 设置显示字符串
	TrySetDisplayTextFromStringValue(CurrentStringValue);

	// 如果存在数据动态设置器，则更新值并通知修改
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this);
	}
}

void UListDataObject_String::OnRotatorInitiatedValueChange(const FText& InNewSelectedText)
{
	// 查找新选择文本在可用选项中的索引
	const int32 FoundIndex = AvailableOptionsTextArray.IndexOfByPredicate(
		[InNewSelectedText](const FText& AvailableText)->bool
		{
			return AvailableText.EqualTo(InNewSelectedText);
		});

	// 如果找到了有效的索引
	if (FoundIndex != INDEX_NONE && AvailableOptionsStringArray.IsValidIndex(FoundIndex))
	{
		// 更新当前显示文本
		CurrentDisplayText = InNewSelectedText;
		
		// 更新当前字符串值
		CurrentStringValue = AvailableOptionsStringArray[FoundIndex];
		
		// 如果存在数据动态设置器，则更新值并通知修改
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			NotifyListDataModified(this);
		}
	}
}

void UListDataObject_String::OnDataObjectInitialized()
{
	// 如果可用选项字符串数组不为空，则将当前值设为第一个选项
	if (!AvailableOptionsStringArray.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStringArray[0];
	}

	// 如果有默认值，则将当前值设为默认值
	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();
	}
	
	// 如果存在数据动态获取器
	if (DataDynamicGetter)
	{
		// 如果获取器中的值不为空，则将当前值设为获取器中的值
		if (!DataDynamicGetter->GetValueAsString().IsEmpty())
		{
			CurrentStringValue = DataDynamicGetter->GetValueAsString();
		}
	}
	
	// 尝试根据字符串值设置显示文本，如果失败则显示"Invalid Value"
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Invalid Value"));
	}
}

bool UListDataObject_String::CanResetBackToDefaultValue() const
{
	// 检查是否有默认值且当前值不等于默认值
	return HasDefaultValue() && CurrentStringValue != GetDefaultValueAsString();
}

bool UListDataObject_String::TryResetBackToDefaultValue()
{
	// 检查是否可以重置回默认值
	if (CanResetBackToDefaultValue())
	{
		// 将当前值设为默认值
		CurrentStringValue = GetDefaultValueAsString();
		
		// 尝试根据字符串值设置显示文本
		TrySetDisplayTextFromStringValue(CurrentStringValue);
		
		// 如果存在数据动态设置器，则更新值并通知修改
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
			return true;
		}
	}
	return false;
}

bool UListDataObject_String::CanSetToForcedStringValue(const FString& InForcedValue) const
{
	// 检查当前值是否不等于强制值
	return CurrentStringValue != InForcedValue;
}

void UListDataObject_String::OnSetToForcedStringValue(const FString& InForcedValue)
{
	// 设置当前字符串值为强制值
	CurrentStringValue = InForcedValue;
	
	// 尝试根据字符串值设置显示文本
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	
	// 如果存在数据动态设置器，则更新值并通知修改（原因是依赖数据修改）
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	}
}

bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	// 获取当前字符串值在可用选项中的索引
	const int32 CurrentFoundIndex = AvailableOptionsStringArray.IndexOfByKey(InStringValue);
	
	// 如果索引有效，则设置显示文本并返回true
	if (AvailableOptionsTextArray.IsValidIndex(CurrentFoundIndex))
	{
		CurrentDisplayText = AvailableOptionsTextArray[CurrentFoundIndex];
		return true;
	}
	
	// 如果索引无效，则返回false
	return false;
}

// ********************  UListDataObject_StringBool  **********************

void UListDataObject_StringBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
	// 如果可用选项字符串数组不包含"True"，则添加真值选项（使用新的显示文本）
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString, InNewTrueDisplayText);
	}
}

void UListDataObject_StringBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
	// 如果可用选项字符串数组不包含"false"，则添加假值选项（使用新的显示文本）
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString, InNewFalseDisplayText);
	}
}

void UListDataObject_StringBool::SetTrueAsDefaultValue()
{
	// 将"True"设置为默认值字符串
	SetDefaultValueFromString(TrueString);
}

void UListDataObject_StringBool::SetFalseAsDefaultValue()
{
	// 将"false"设置为默认值字符串
	SetDefaultValueFromString(FalseString);
}

void UListDataObject_StringBool::OnDataObjectInitialized()
{
	// 尝试初始化布尔值选项
	TryInitBoolValues();
	Super::OnDataObjectInitialized();
}

void UListDataObject_StringBool::TryInitBoolValues()
{
	// 如果可用选项字符串数组不包含"True"，则添加真值选项
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString, FText::FromString(TEXT("ON")));
	}
	
	// 如果可用选项字符串数组不包含"false"，则添加假值选项
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString, FText::FromString(TEXT("OFF")));
	}
}

void UListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
	// 将整数值转换为字符串并添加为动态选项
	AddDynamicOption(LexToString(InIntegerValue), InDisplayText);
}

void UListDataObject_StringInteger::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	// 尝试根据字符串值设置显示文本，如果失败则显示"自定义"
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("自定义"));
	}
}

void UListDataObject_StringInteger::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	// 如果存在数据动态获取器
	if (DataDynamicGetter)
	{
		// 如果两个数据相互依赖，为了避免无限循环 当两个数据一致时返回
		if (CurrentStringValue == DataDynamicGetter->GetValueAsString()) return;
		
		// 更新当前字符串值为获取器中的值
		CurrentStringValue = DataDynamicGetter->GetValueAsString();
		
		// 尝试根据字符串值设置显示文本，如果失败则显示"自定义"
		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			CurrentDisplayText = FText::FromString(TEXT("自定义"));
		}
		
		// 通知列表数据已修改，原因是依赖数据修改
		NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	}
	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}

