// 幻雨喜欢小猫咪


#include "FrontendCommonListView.h"

#include "Editor/WidgetCompilerLog.h"
#include "FrontendUI/Widgets/Options/DataAsset_DataListEntryMapping.h"
#include "FrontendUI/Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "FrontendUI/Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "FrontendUI/Widgets/Options/ListEntries/Widget_ListEntry_Base.h"

UUserWidget& UFrontendCommonListView::OnGenerateEntryWidgetInternal(UObject* Item,
                                                                    TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	// 在编辑器设计模式（Slate 编辑器或 UMG 预览）下，使用父类默认行为，避免运行自定义逻辑导致崩溃
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}

	// 安全地将 Item 转为选项数据对象基类（选项列表中只会放这类对象）
	UListDataObject_Base* DataObject = CastChecked<UListDataObject_Base>(Item);

	// 通过映射表查找该数据对象对应的专用条目控件类
	if (TSubclassOf<UWidget_ListEntry_Base> FoundWidgetClass = 
		DataListEntryMapping->FindEntryWidgetClassByDataObject(DataObject))
	{
		// 找到匹配的控件类 → 使用它生成条目（GenerateTypedEntry 是 CommonListView 提供的模板函数）
		return GenerateTypedEntry<UWidget_ListEntry_Base>(FoundWidgetClass, OwnerTable);
	}
	
	// 未找到映射 → 回退到父类默认行为（通常是使用蓝图中配置的 Entry Widget Class）
	// 这种情况在开发中应视为警告，可能需要补充映射表
	return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
}

bool UFrontendCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	// 如果当前选中（或尝试选中）的第一个项是“集合”类型，则不允许选中/导航
	// 这样玩家手柄上下移动时会自动跳过这些标题行，保持良好体验
	return !FirstSelectedItem->IsA<UListDataObject_Collection>();
}

#if WITH_EDITOR
/**
 * 编辑器下蓝图编译时的默认值校验
 * 强制要求开发者配置 DataListEntryMapping，避免运行时出现“找不到控件类”的问题
 */
void UFrontendCommonListView::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!DataListEntryMapping)
	{
		CompileLog.Error(FText::FromString(
			TEXT("FrontendCommonListView: DataListEntryMapping 未配置！必须指定一个有效的 UDataAsset_DataListEntryMapping 资源，否则选项列表无法正确生成条目控件。")
		));
	}
}
#endif
