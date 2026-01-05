// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FrontendCommonButtonBase.generated.h"

class UCommonLazyImage;
class UCommonTextBlock;

/**
 * 前端通用按钮基础类，继承自UCommonButtonBase
 * 提供了按钮文本和图像的设置功能，以及按钮悬停时的描述文本处理
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class UFrontendCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
public:
	/**
	 * 设置按钮显示文本
	 * @param InText 要设置的文本内容
	 */
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText);
	
	/**
	 * 获取按钮显示文本
	 * @return 按钮当前显示的文本内容
	 */
	UFUNCTION(BlueprintCallable)
	FText GetButtonDisplayText() const;

	/**
	 * 设置按钮显示图像
	 * @param InBrush 要设置的图像画刷
	 */
	UFUNCTION(BlueprintCallable)
	void SetButtonDisplayImage(const FSlateBrush& InBrush);
	
private:
	// begin UUserWidget Interface
	/** 构造前的本地初始化 */
	virtual void NativePreConstruct() override;
	// end UUserWidget Interface

	// begin UCommonButtonBase Interface
	/** 当前文本样式改变时的处理 */
	virtual void NativeOnCurrentTextStyleChanged() override;
	/** 鼠标悬停时的处理 */
	virtual void NativeOnHovered() override;
	/** 鼠标离开时的处理 */
	virtual void NativeOnUnhovered() override;
	// end UCommonButtonBase Interface

	// ****Bound Widgets ****//  bound  边界  optional  可选择的，选修的
	/** 按钮文本显示组件 */
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_ButtonText;

	/** 按钮图像显示组件 */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess=true))
	UCommonLazyImage* CommonLazyImage_ButtonImage;
	// ****Bound Widgets ****//

	/** 按钮显示文本 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Button", meta = (AllowPrivateAccess = "true", DisplayName = "按钮显示文本"))
	FText ButtonDisplayText;

	/** 是否将按钮文本转换为大写 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Button", meta = (AllowPrivateAccess = "true", DisplayName = "按钮文本转大写"))
	bool bUserUpperCaseForButtonText = false;

	/** 按钮描述文本，当鼠标悬停时会触发 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Button", meta = (AllowPrivateAccess = "true", DisplayName = "按钮描述文本"))
	FText ButtonDescriptionText;
};
