// 幻雨喜欢小猫咪


#include "ItemWidget.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 允许控件获得焦点
	SetIsFocusable(true);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(IconTexture);
	}
}

UItemToolTip* UItemWidget::SetToolTipWidget(const UPDA_ShopItem* Item)
{
	if (!Item) return nullptr;

	if (GetOwningPlayer() && ItemToolTipClass)
	{
		// 创建提示信息控件实例
		UItemToolTip* ToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(), ItemToolTipClass);
		if (ToolTip)
		{
			ToolTip->SetItem(Item);	// 设置提示信息
			// 添加提示信息（调用该函数后才能实现把控件信息跟着鼠标走）
			SetToolTip(ToolTip);	// 关联到当前控件
		}
		return ToolTip;
	}
	return nullptr;
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 父类的按下处理
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// 按下的是右键
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// 设置控件焦点
		return FReply::Handled().SetUserFocus(TakeWidget());
	}

	// 左按按下
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return FReply::Handled()
		.SetUserFocus(TakeWidget())
		.DetectDrag(TakeWidget(),EKeys::LeftMouseButton); // 拖拽
	}
	return SuperReply; // 返回父类处理
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	// 仅当控件当前有焦点时处理点击事件（避免误触其它控件）
	if (HasAnyUserFocus())
	{
		// 右键释放：触发右键点击事件
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			RightButtonClicked();	// 执行右键的逻辑
			return FReply::Handled();
		}

		// 左键释放：触发左键点击事件
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			LeftButtonClicked(); // 执行左键逻辑
			return FReply::Handled(); // 标记事件已处理
		}
	}
	return SuperReply;
}

void UItemWidget::RightButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("按下右键"));
}

void UItemWidget::LeftButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("按下左键"));
}
