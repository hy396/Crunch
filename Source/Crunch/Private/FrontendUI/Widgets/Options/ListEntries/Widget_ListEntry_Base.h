// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserListEntry.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "FrontendUI/FrontendTypes/FrontendEnumTypes.h"
#include "Widget_ListEntry_Base.generated.h"

class UListDataObject_Base;
class UCommonTextBlock;
/**
 * 列表条目基础控件类
 * 抽象类，用于创建选项列表中的各个条目控件
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class CRUNCH_API UWidget_ListEntry_Base : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	// 蓝图可实现事件，当列表条目控件悬停状态改变时调用
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On list Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered, bool bIsEntryWidgetStillSelected);
	
	/**
	 * 当列表条目控件悬停状态改变时的原生实现
	 * @param bWasHovered 悬停状态是否改变
	 */
	void NativeOnListEntryWidgetHovered(bool bWasHovered);
protected:
	// begin IUserObjectListEntry
	/**
	 * 当列表项对象设置时调用
	 * @param ListItemObject 列表项对象
	 */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	// 当条目被释放时调用（解决切换tab时多个entry高亮问题）
	virtual void NativeOnEntryReleased() override;
	
	/**
	 * 当条目选择状态改变时调用
	 * @param bIsSelected 是否被选中
	 */
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	// end IUserObjectListEntry

	/**
	 * 当拥有的列表数据对象设置时调用
	 * @param InOwningListDataObject 拥有的列表数据对象
	 */
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);

	/**
	 * 当拥有的列表数据对象修改时调用
	 * @param OwningModifiedData 拥有的修改后的数据对象
	 * @param ModifyReason 修改原因
	 */
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason);

	/**
	 * 当拥有的依赖数据对象修改时调用
	 * @param OwningModifiedData 拥有的修改后的数据对象
	 * @param ModifyReason 修改原因
	 */
	virtual void OnOwningDependencyDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason);
	
	// 选择此条目控件
	void SelectThisEntryWidget();

	// 手柄使用必须重写函数   begin UUserWidget interface
	/**
	 * 当焦点接收时调用（手柄支持）
	 * @param InGeometry 几何体信息
	 * @param InFocusEvent 焦点事件
	 * @return 返回处理结果
	 */
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	// end UUserWidget interface

	// 蓝图可实现事件，获取手柄焦点控件
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;
	
	/**
	 * 当切换可编辑状态时调用
	 * @param bIsEditable 是否可编辑
	 */
	virtual void OnToggleEditableState(bool bIsEditable);

	// 蓝图可实现事件，当切换条目控件高亮状态时调用
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Toggle Entry Widget Highlight State"))
	void BP_OnToggleEntryWidgetHighlightState(bool bShouldHighlight) const;
	
private:
	// ****需要绑定的控件
	// 设置显示名称的文本控件
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess=true))
	UCommonTextBlock* CommonText_SettingDisplayName;
	// ****需要绑定的控件

	// 缓存的拥有数据对象
	UPROPERTY(Transient)
	UListDataObject_Base* CachedOwningDataObject;
};
