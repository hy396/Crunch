// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "WaitingWidget.generated.h"

/**
 * 等待界面控件
 * 用于在需要玩家等待（如连接、加载、匹配）时显示提示信息，
 * 并可选提供“取消”按钮。
 */
UCLASS()
class CRUNCH_API UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	// 获取 Cancel 按钮的点击事件引用，并清空之前绑定的所有回调。
	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent();

	/**
	 * 设置等待提示信息，并控制“取消”按钮是否可见。
	 * @param WaitInfo   等待提示文本
	 * @param bAllowCancel 是否允许取消（决定按钮显隐）
	 */
	void SetWaitInfo(const FText& WaitInfo, bool bAllowCancel = false);

private:
	// 等待提示文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WaitInfoText;

	// 取消按钮
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
};
