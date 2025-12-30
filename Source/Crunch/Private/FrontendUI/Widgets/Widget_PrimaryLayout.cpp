// 幻雨喜欢小猫咪


#include "Widget_PrimaryLayout.h"
#include "HuanYuDebugHelper.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	checkf(RegisteredWidgetStackMap.Contains(InTag), TEXT("通过标签%s找不到widget stack"), *InTag.ToString());
	return RegisteredWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	if (!IsDesignTime())  // IsDesignTime() 检测当前是否处于编辑器设计模式（非游戏运行模式）：返回 true：代码在编辑器界面中执行（如编辑关卡、调整属性时）返回 false：代码在游戏运行时执行（包括 PIE 模式、独立运行等）
	{
		if (!RegisteredWidgetStackMap.Contains(InStackTag))
		{
			RegisteredWidgetStackMap.Add(InStackTag, InStack);

			// Debug::Print(TEXT("widget stack registered under the tag--") + InStackTag.ToString());
		}
	}
}
