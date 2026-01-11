// 幻雨喜欢小猫咪


#include "Widget_ListEntry_Base.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"
#include "FrontendUI/Widgets/Options/DataObjects/ListDataObject_Base.h"

void UWidget_ListEntry_Base::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
	// 当列表项被悬停时的处理逻辑
	// BP_OnListEntryWidgetHovered(bWasHovered, IsListItemSelected());最开始这个版本修改fps重置在修改在重置会崩溃
	BP_OnListEntryWidgetHovered(bWasHovered, GetListItem()?IsListItemSelected() : false);
	if (bWasHovered)
	{
		// 悬停时高亮显示
		BP_OnToggleEntryWidgetHighlightState(true);
	}else
	{
		// 非悬停时根据选中状态决定是否高亮
		BP_OnToggleEntryWidgetHighlightState(GetListItem() && IsListItemSelected());
	}
}

void UWidget_ListEntry_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	// TODO: 这个后面被注释了很诡异(已解决，这是在蓝图里为其单独设置，无敌了）
	// SetVisibility(ESlateVisibility::Visible);
	OnOwningListDataObjectSet(CastChecked<UListDataObject_Base>(ListItemObject));
}

void UWidget_ListEntry_Base::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();
	NativeOnListEntryWidgetHovered(false);
}

void UWidget_ListEntry_Base::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
	BP_OnToggleEntryWidgetHighlightState(bIsSelected);
}

void UWidget_ListEntry_Base::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	// 当拥有列表数据对象设置时的处理
	if (CommonText_SettingDisplayName)
	{
		// 设置显示名称文本
		CommonText_SettingDisplayName->SetText(InOwningListDataObject->GetDataDisplayName());
	}

	if (!InOwningListDataObject->OnListDataModified.IsBoundToObject(this))
	{
		// 绑定数据修改事件处理器
		InOwningListDataObject->OnListDataModified.AddUObject(this, &ThisClass::OnOwningListDataObjectModified);
	}

	if (!InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this))
	{
		// 绑定依赖数据修改事件处理器
		InOwningListDataObject->OnDependencyDataModified.AddUObject(this, &ThisClass::OnOwningDependencyDataObjectModified);
	}

	// 切换可编辑状态
	OnToggleEditableState(InOwningListDataObject->IsDataCurrentlyEditable());
	CachedOwningDataObject = InOwningListDataObject;
}

void UWidget_ListEntry_Base::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
}

void UWidget_ListEntry_Base::OnOwningDependencyDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningDataObject)
	{
		// 刷新可编辑状态
		OnToggleEditableState(CachedOwningDataObject->IsDataCurrentlyEditable());
	}
}

void UWidget_ListEntry_Base::SelectThisEntryWidget()
{
	// 获取列表视图
	CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}

FReply UWidget_ListEntry_Base::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	// 检查是否为游戏手柄输入模式
	if (CommonInputSubsystem && CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		// 获取手柄输入时需要聚焦的控件
		if (UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
		{
			// 获取Slate底层控件并设置焦点
			if (TSharedPtr<SWidget> SlateWidgetToFocus = WidgetToFocus->GetCachedWidget())
			{
				return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
			}
		}
	}
	// 调用父类的焦点处理逻辑
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UWidget_ListEntry_Base::OnToggleEditableState(bool bIsEditable)
{
	if (CommonText_SettingDisplayName)
	{
		// 设置当前控件的启用状态
		CommonText_SettingDisplayName->SetIsEnabled(bIsEditable);
	}
}
