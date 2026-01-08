// 幻雨喜欢小猫咪


#include "FrontendCommonRotator.h"

#include "CommonTextBlock.h"

void UFrontendCommonRotator::SetSelectedOptionByText(const FText& InTextOption)
{
	// 通过文本查找对应的选项索引
	const int32 FoundIndex = TextLabels.IndexOfByPredicate([InTextOption](const FText& TextItem)->bool
	{
		return TextItem.EqualTo(InTextOption);
	});
	
	if (FoundIndex != INDEX_NONE)
	{
		// 如果找到匹配的选项，则设置为选中状态
		SetSelectedItem(FoundIndex);
	}
	else
	{
		// 如果未找到匹配选项，则直接设置文本内容
		MyText->SetText(InTextOption);
	}
}
