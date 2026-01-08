// 幻雨喜欢小猫咪


#include "Widget_OptionsDetailsView.h"

#include "CommonLazyImage.h"
#include "CommonRichTextBlock.h"
#include "CommonTextBlock.h"
#include "DataObjects/ListDataObject_Base.h"

void UWidget_OptionsDetailsView::UpdateDetailsViewInfo(UListDataObject_Base* InDataObject,
                                                       const FString& InEntryWidgetClassName)
{
	if (!InDataObject) return;

	// 1. 设置标题
	CommonTextBlock_Title->SetText(InDataObject->GetDataDisplayName());

	// 2. 处理描述图片（如果配置了软纹理引用）
	if (!InDataObject->GetSoftDescriptionImage().IsNull())
	{
		// 使用懒加载方式设置图片（运行时才加载，避免打包体积过大）
		CommonLazyImage_DescriptionImage->SetBrushFromLazyTexture(InDataObject->GetSoftDescriptionImage());
		// SelfHitTestInvisible：图片可见但不拦截输入（允许点击下方控件）
		CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		// 没有图片时完全隐藏
		CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 3. 设置主要富文本描述（支持加粗、颜色等样式）
	CommonRichText_Description->SetText(InDataObject->GetDescriptionRichText());
// 只在编辑器模式下显示动态调试信息
// #if WITH_EDITOR	
	// 4. 生成动态调试信息（仅在开发时有用，帮助排查控件对应关系）
	const FString DynamicDetails = FString::Printf(
		TEXT("Data Object Class: <Bold>%s</>\n\nEntry Widget Class: <Bold>%s</>"),
		*InDataObject->GetClass()->GetName(),
		*InEntryWidgetClassName
	);
	CommonRichText_DynamicDetails->SetText(FText::FromString(DynamicDetails));
// #endif 
	// 5. 显示禁用原因（如果当前选项不可编辑）
	if (InDataObject->IsDataCurrentlyEditable())
	{
		// 可编辑 → 不显示任何禁用说明
		CommonRichText_DisabledReason->SetText(FText::GetEmpty());
	}
	else
	{
		// 不可编辑 → 显示配置的富文本原因（如“此设置需重启游戏后生效”）
		CommonRichText_DisabledReason->SetText(InDataObject->GetDisabledRichText());
	}
}

void UWidget_OptionsDetailsView::ClearDetailsViewInfo()
{
	CommonTextBlock_Title->SetText(FText::GetEmpty());
	CommonLazyImage_DescriptionImage->SetVisibility(ESlateVisibility::Collapsed);
	CommonRichText_Description->SetText(FText::GetEmpty());
	CommonRichText_DynamicDetails->SetText(FText::GetEmpty());
	CommonRichText_DisabledReason->SetText(FText::GetEmpty());
}

void UWidget_OptionsDetailsView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 初始化时清空所有内容，避免蓝图中残留测试文本
	ClearDetailsViewInfo();
}
