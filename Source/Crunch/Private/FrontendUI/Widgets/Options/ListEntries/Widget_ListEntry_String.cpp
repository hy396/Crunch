// 幻雨喜欢小猫咪


#include "Widget_ListEntry_String.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "FrontendUI/Widgets/Components/FrontendCommonButtonBase.h"
#include "FrontendUI/Widgets/Components/FrontendCommonRotator.h"
#include "FrontendUI/Widgets/Options/DataObjects/ListDataObject_String.h"
// #include "HuanYuDebugHelper.h"

void UWidget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 绑定左右箭头按钮的点击事件
	CommonButton_PreviousOption->OnClicked().AddUObject(this, &ThisClass::OnPreviousOptionButtonClicked);
	CommonButton_NextOption->OnClicked().AddUObject(this, &ThisClass::OnNextOptionButtonClicked);

	// 点击中间旋转器区域时，让当前条目获得焦点选中（用于手柄导航高亮）
	CommonRotator_AvailableOptions->OnClicked().AddLambda([this]() { SelectThisEntryWidget(); });

	// 绑定旋转器值变化事件（主要用于手柄左右键快速切换时）
	CommonRotator_AvailableOptions->OnRotatedEvent.AddUObject(this, &ThisClass::OnRotatorValueChanged);
}

void UWidget_ListEntry_String::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	// 缓存为字符串类型数据对象（使用 CastChecked 确保类型安全）
	CachedOwningStringDataObject = CastChecked<UListDataObject_String>(InOwningListDataObject);

	// 向旋转器填充所有可用的选项文本（如 "低", "中", "高", "史诗"）
	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());

	// 根据当前选中的值设置旋转器显示的文本
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
}

void UWidget_ListEntry_String::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	// Super::OnOwningListDataObjectModified(OwningModifiedData, ModifyReason);
	if (CachedOwningStringDataObject)
	{
		// 刷新旋转器显示为最新值
		CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
	}
}

void UWidget_ListEntry_String::OnToggleEditableState(bool bIsEditable)
{
	Super::OnToggleEditableState(bIsEditable);
	
	CommonButton_PreviousOption->SetIsEnabled(bIsEditable);
	CommonRotator_AvailableOptions->SetIsEnabled(bIsEditable);
	CommonButton_NextOption->SetIsEnabled(bIsEditable);
}

void UWidget_ListEntry_String::OnPreviousOptionButtonClicked()
{
	// Debug::Print(TEXT("上一个按钮被点击"));
	if (CachedOwningStringDataObject)
	{
		// 调用数据对象的逻辑切换到上一项（内部会循环）
		CachedOwningStringDataObject->BackToPreviousOption();
	}
	// 当点击上一个按钮时，高亮状态改变
	SelectThisEntryWidget();
}

void UWidget_ListEntry_String::OnNextOptionButtonClicked()
{
	// Debug::Print(TEXT("下一个按钮被点击"));
	if (CachedOwningStringDataObject)
	{
		// 调用数据对象的逻辑切换到下一项（内部会循环）
		CachedOwningStringDataObject->AdvanceToNextOption();
	}
	// 当点击下一个按钮时，高亮状态改变
	SelectThisEntryWidget();
}

void UWidget_ListEntry_String::OnRotatorValueChanged(int32 Value, bool bUserInitiated)
{
	if (!CachedOwningStringDataObject) return;

	// 获取当前输入设备类型（鼠标/键盘/手柄）
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	if (!CommonInputSubsystem || !bUserInitiated) return;

	// 只有在使用手柄时，才需要特殊处理（可能是为了触发保存或音效等）
	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		// 通知数据对象：通过旋转器方式改变了值（可能用于统计或音效）
		CachedOwningStringDataObject->OnRotatorInitiatedValueChange(CommonRotator_AvailableOptions->GetSelectedText());
	}
}
