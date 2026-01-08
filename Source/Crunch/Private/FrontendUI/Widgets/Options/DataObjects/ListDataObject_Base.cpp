// 幻雨喜欢小猫咪


#include "ListDataObject_Base.h"

#include "FrontendUI/FrontendSettings/FrontendGameUserSettings.h"

void UListDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void UListDataObject_Base::AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition)
{
	// 将编辑条件添加到数组中
	EditConditionDescArray.Add(InEditCondition);
}

bool UListDataObject_Base::IsDataCurrentlyEditable()
{
	// 默认认为数据是可编辑的
	bool bIsEditable = true;
	
	// 如果没有编辑条件，则直接返回true
	if (EditConditionDescArray.IsEmpty()) return bIsEditable;

	// 缓存禁用原因的字符串
	FString CachedDisabledRichReason;
	
	// 遍历所有编辑条件
	for (const FOptionsDataEditConditionDescriptor& Condition : EditConditionDescArray)
	{
		// 如果条件无效或者条件满足，则跳过
		if (!Condition.IsValid() || Condition.IsEditConditionMet()) continue;
		
		// 条件不满足，标记为不可编辑
		bIsEditable = false;
		
		// 追加禁用原因到缓存字符串
		CachedDisabledRichReason.Append(Condition.GetDisabledRichReason());
		
		// 设置禁用状态下的富文本提示
		SetDisabledRichText(FText::FromString(CachedDisabledRichReason));

		// 如果有条件指定的强制字符串值
		if (Condition.HasForcedStringValue())
		{
			// 获取强制字符串值
			const FString ForcedStringValue = Condition.GetDisabledForcedStringValue();
			
			// 检查是否可以设置为强制字符串值
			if (CanSetToForcedStringValue(ForcedStringValue))
			{
				// 设置为强制字符串值
				OnSetToForcedStringValue(ForcedStringValue);
			}
		}
	}
	
	// 返回是否可编辑的结果
	return bIsEditable;
}

void UListDataObject_Base::AddEditDependencyData(UListDataObject_Base* InDependencyData)
{
	// 检查依赖数据对象的修改委托是否已经绑定到当前对象
	if (!InDependencyData->OnListDataModified.IsBoundToObject(this))
	{
		// 将当前对象的OnEditDependencyDataModified方法绑定到依赖数据的修改委托上
		InDependencyData->OnListDataModified.AddUObject(this, &ThisClass::OnEditDependencyDataModified);
	}
}

void UListDataObject_Base::OnDataObjectInitialized()
{
	// 基类实现为空，供子类重写
}

void UListDataObject_Base::NotifyListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	// 广播数据修改委托
	OnListDataModified.Broadcast(ModifiedData, ModifyReason);

	// 如果设置了应立即应用更改
	if (bShouldApplyChangeImmediately)
	{
		// 应用游戏设置
		UFrontendGameUserSettings::Get()->ApplySettings(true);
	}
}

void UListDataObject_Base::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	// 广播依赖数据修改委托
	OnDependencyDataModified.Broadcast(ModifiedDependencyData, ModifyReason);
}
