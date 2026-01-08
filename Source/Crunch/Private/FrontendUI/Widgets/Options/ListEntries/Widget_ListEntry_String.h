// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_String.generated.h"

class UListDataObject_String;               // 字符串类型选项数据对象
class UFrontendCommonRotator;               // 前端通用旋转器控件（带左右箭头和文本显示）
class UFrontendCommonButtonBase;            // 前端通用按钮基类

/**
 * 字符串类型列表条目控件
 * 用于选项界面中显示“可从多个字符串选项中选择一个”的设置项
 * 典型外观：左箭头按钮 + 中间显示当前选项文本 + 右箭头按钮（类似下拉菜单的旋转器样式）
 * 示例：图形质量（低/中/高/史诗）、语言选择、分辨率等
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_ListEntry_String : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
protected:
	// ==================== UUserWidget 接口 ====================
	// Widget 初始化完成时调用，用于绑定子控件事件
	virtual void NativeOnInitialized() override;
	// ===========================================================

	// ==================== UWidget_ListEntry_Base 接口 ====================
	// 当该条目被绑定到一个列表数据对象时调用，用于初始显示内容
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;

	// 当绑定的数据对象内容发生变化时调用，用于刷新显示
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, 
		EOptionsListDataModifyReason ModifyReason) override;

	// 当整个选项界面切换可编辑状态（如预览模式 vs 编辑模式）时调用
	virtual void OnToggleEditableState(bool bIsEditable) override;
	// =====================================================================
	
private:
	// ====================== 绑定的子控件 ======================
	// 左箭头按钮：切换到上一个选项
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonButtonBase* CommonButton_PreviousOption;

	// 中间旋转器：显示当前选中的选项文本，并提供所有可用选项
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonRotator* CommonRotator_AvailableOptions;

	// 右箭头按钮：切换到下一个选项
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonButtonBase* CommonButton_NextOption;
	// ===========================================================

	// 缓存当前绑定的字符串类型数据对象，避免频繁类型转换
	UPROPERTY(Transient)
	UListDataObject_String* CachedOwningStringDataObject;

	// 左箭头按钮点击回调
	void OnPreviousOptionButtonClicked();

	// 右箭头按钮点击回调
	void OnNextOptionButtonClicked();

	/**
	 * 旋转器值发生变化时的回调（手柄或键盘操作时触发）
	 * @param Value 新的值
	 * @param bUserInitiated 是否由用户操作触发
	 */
	void OnRotatorValueChanged(int32 Value, bool bUserInitiated);
};
