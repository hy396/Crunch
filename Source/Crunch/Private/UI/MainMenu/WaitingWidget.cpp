// 幻雨喜欢小猫咪


#include "WaitingWidget.h"

void UWaitingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedEvent()
{
	// 清空按钮已有的点击事件
	CancelButton->OnClicked.Clear();
	return CancelButton->OnClicked;
}

void UWaitingWidget::SetWaitInfo(const FText& WaitInfo, bool bAllowCancel)
{
	// 设置取消按钮的可见性
	if (CancelButton)
	{
		CancelButton->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	// 更新提示文字
	if (WaitInfoText)
	{
		WaitInfoText->SetText(WaitInfo);
	}
}
