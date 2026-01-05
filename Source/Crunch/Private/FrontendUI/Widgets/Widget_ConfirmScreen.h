// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "FrontendUI/FrontendTypes/FrontendEnumTypes.h"
#include "Widget_ConfirmScreen.generated.h"

class UDynamicEntryBox;
class UCommonTextBlock;

/**
 * 确认屏幕按钮信息结构体
 * 用于定义确认屏幕中按钮的类型和显示文本
 */
USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	/** 按钮类型，决定按钮点击后的行为 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::Unknown;

	/** 按钮上显示的文本 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonTextToDisplay;
	
};

/**
 * 确认屏幕信息对象类
 * 用于创建和管理确认屏幕的标题、消息和按钮信息
 */
UCLASS()
class CRUNCH_API UConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 创建一个只包含"OK"按钮的确认屏幕
	 * @param InScreenTitle 屏幕标题
	 * @param InScreenMessage 屏幕消息
	 * @return 确认屏幕信息对象
	 */
	static UConfirmScreenInfoObject* CreateOKScreen(const FText& InScreenTitle, const FText& InScreenMessage);

	/**
	 * 创建一个包含"Yes/No"按钮的确认屏幕
	 * @param InScreenTitle 屏幕标题
	 * @param InScreenMessage 屏幕消息
	 * @return 确认屏幕信息对象
	 */
	static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMessage);

	/**
	 * 创建一个包含"OK/Cancel"按钮的确认屏幕
	 * @param InScreenTitle 屏幕标题
	 * @param InScreenMessage 屏幕消息
	 * @return 确认屏幕信息对象
	 */
	static UConfirmScreenInfoObject* CreateOKCancelScreen(const FText& InScreenTitle, const FText& InScreenMessage);
	
	/** 屏幕标题 */
	UPROPERTY(Transient)
	FText ScreenTitle;

	/** 屏幕消息 */
	UPROPERTY(Transient)
	FText ScreenMessage;

	/** 可用的屏幕按钮列表 */
	UPROPERTY(Transient)
	TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;  // 可用屏幕按钮
};

/**
 * 确认屏幕控件基类
 * 提供通用的确认对话框功能，包含标题、消息和动态按钮
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_ConfirmScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
public:
	/**
	 * 初始化确认屏幕
	 * @param InScreenInfoObject 屏幕信息对象，包含标题、消息和按钮信息
	 * @param ClickedButtonCallback 按钮点击后的回调函数，传入点击的按钮类型
	 */
	void InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback);

protected:
	// begin UCommonActivatableWidget Interface
	/** 
	 * 获取期望的焦点目标
	 * 解决重置弹窗关闭时手柄不聚焦无法导航问题
	 */
	virtual UWidget* NativeGetDesiredFocusTarget() const override; // 解决重置弹窗关闭时手柄不聚焦无法导航问题
	// end UCommonActivatableWidget Interface
private:
	/** 绑定标题文本控件 */
	UPROPERTY(meta=(BindWidget))
	UCommonTextBlock* CommonTextBlock_Title;

	/** 绑定消息文本控件 */
	UPROPERTY(meta=(BindWidget))
	UCommonTextBlock* CommonTextBlock_Message;

	/** 绑定动态按钮框控件 */
	UPROPERTY(meta=(BindWidget))
	UDynamicEntryBox* DynamicEntryBox_Buttons;
};
