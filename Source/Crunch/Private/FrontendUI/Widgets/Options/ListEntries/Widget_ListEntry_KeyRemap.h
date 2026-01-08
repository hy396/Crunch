// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_KeyRemap.generated.h"

class UListDataObject_KeyRemap;
class UFrontendCommonButtonBase;
/**
 * 键位映射列表项控件类
 * 用于处理用户界面中的键位重新映射功能，继承自基础列表项控件
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class CRUNCH_API UWidget_ListEntry_KeyRemap : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
protected:
	//~ begin UWidget_ListEntry_Base
	/**
	 * 当设置拥有者列表数据对象时调用
	 * @param InOwningListDataObject 输入的拥有者列表数据对象
	 */
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;

	/**
	 * 当拥有者列表数据对象被修改时调用
	 * @param OwningModifiedData 被修改的拥有者数据对象
	 * @param ModifyReason 修改原因
	 */
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;
	//~ end UWidget_ListEntry_Base

	// begin UUserWidget Interface
	/**
	 * 初始化原生控件时调用
	 */
	virtual void NativeOnInitialized() override;
	// end UUserWidget Interface
private:
	/** 重新映射键按钮 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	UFrontendCommonButtonBase* CommonButton_RemapKey;

	/** 重置键绑定按钮 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess=true))
	UFrontendCommonButtonBase* CommonButton_ResetKeyBinding;

	/** 缓存的拥有者键映射数据对象 */
	UPROPERTY(Transient)
	UListDataObject_KeyRemap* CachedOwningKeyRemapDataObject;

	/** 重新映射键按钮点击事件处理 */
	void OnRemapKeyButtonClicked();

	/** 重置键绑定按钮点击事件处理 */
	void OnResetKeyBindingButtonClicked();

	// 代理回调
	/** 键重新映射按下事件处理 */
	void OnKeyRemapPressed(const FKey& PressedKey);

	/** 键重新映射取消事件处理 */
	void OnKeyRemapCanceled(const FString& CanceledReason);
};
