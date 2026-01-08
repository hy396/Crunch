// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_Scalar.generated.h"

class UListDataObject_Scalar;
class UAnalogSlider;
class UCommonNumericTextBlock;

/**
 * 标量类型列表条目控件类
 * 用于显示和操作标量数值类型的选项条目
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class CRUNCH_API UWidget_ListEntry_Scalar : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
protected:
	// begin UUserWidget Interface
	/**
	 * @brief 在UMG控件完成初始化后调用的原生函数。
	 * 
	 * 该函数负责执行一次性的初始化逻辑，例如绑定委托（Delegate）到滑块控件的事件上。
	 * 确保在尝试访问任何绑定的子控件之前，所有控件都已创建完毕。
	 */
	virtual void NativeOnInitialized() override;
	// end UUserWidget Interface
	
	// begin UWidget_ListEntry_Base
	/**
	 * @brief 当为此控件分配了所属的列表数据对象时调用。
	 * 
	 * 该函数负责接收一个基类数据对象指针，将其转换为具体的 `UListDataObject_Scalar` 类型，
	 * 并缓存到 `CachedOwningScalarDataObject` 中。随后会根据该数据对象的当前值来刷新UI显示。
	 * 
	 * @param InOwningListDataObject 指向新设置的列表数据对象的指针。
	 */
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	
	/**
	 * @brief 当所属的列表数据对象内容被外部修改时调用。
	 * 
	 * 该函数作为数据变更的通知机制，当 `CachedOwningScalarDataObject` 的值被其他地方（如代码或另一个UI元素）更改时，
	 * 此函数会被触发。它负责根据新的数据状态刷新UI控件（如数字文本和滑块）的显示。
	 * 
	 * @param OwningModifiedData 指向被修改的数据对象的指针。
	 * @param ModifyReason 描述数据被修改原因的枚举值。
	 */
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;
	// end UWidget_ListEntry_Base

private:
	// ******绑定控件****//
	/**
	 * @brief 显示当前设置数值的通用数字文本控件。
	 * 
	 * 此控件用于以格式化的方式展示 `CachedOwningScalarDataObject` 中存储的标量值。
	 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	UCommonNumericTextBlock* CommonNumeric_SettingValue;

	/**
	 * @brief 允许用户通过拖动来调整标量值的模拟滑块控件。
	 * 
	 * 此控件的值变化会通过 `OnSliderValueChanged` 回调函数进行处理，
	 * 并最终更新到关联的数据对象中。
	 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	UAnalogSlider* AnalogSlider_SettingSlider;
		
	// ******绑定控件****//

	/**
	 * @brief 对拥有此控件实例的 `UListDataObject_Scalar` 数据对象的缓存引用。
	 * 
	 * 当 `OnOwningListDataObjectSet` 被调用时，此指针会被初始化。它避免了在每次访问数据时进行类型转换，
	 * 提高了性能并简化了代码。
	 */
	UPROPERTY(Transient)
	UListDataObject_Scalar* CachedOwningScalarDataObject;

	/**
	 * @brief UMG滑块控件值发生改变时的回调函数。
	 * 
	 * 此函数会在用户拖动 `AnalogSlider_SettingSlider` 时被调用。
	 * 它负责将新的滑块值应用到 `CachedOwningScalarDataObject` 中，并可能触发数据修改事件。
	 * 
	 * @param Value 从滑块控件传入的新浮点数值。
	 */
	UFUNCTION()
	void OnSliderValueChanged(float Value);
	
	/**
	 * @brief UMG滑块控件开始鼠标捕获时的回调函数。
	 * 
	 * 此函数在用户点击并开始拖动 `AnalogSlider_SettingSlider` 时被调用。
	 * 可用于标记交互的开始，例如记录初始值或通知系统进入调整模式。
	 */
	UFUNCTION()
	void OnSliderMouseCaptureBegin();
};
