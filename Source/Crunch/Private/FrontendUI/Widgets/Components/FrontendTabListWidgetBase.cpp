// 幻雨喜欢小猫咪


#include "FrontendTabListWidgetBase.h"

#include "FrontendCommonButtonBase.h"
#include "Editor/WidgetCompilerLog.h"


void UFrontendTabListWidgetBase::RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName)
{
	/**
	 * 调用父类UCommonTabListWidgetBase的核心函数注册新Tab
	 * 参数说明：
	 * - InTabID                    : Tab的唯一标识名
	 * - TabButtonEntryWidgetClass  : 预设的Tab按钮Widget类（通常是UCommonButtonBase的子类，决定了按钮的外观风格）
	 * - TabButtonEntryWidgetClass  : 预设的Tab按钮Widget类（通常是UCommonButtonBase的子类，决定了按钮的外观风格）
	 * - nullptr                    : 这里故意传空，表示不绑定任何内容页面（Content Widget）
	 * 因为选项屏幕的Tab切换是通过代码刷新列表实现的，而不是用Switcher自动切换页面
	 */
	RegisterTab(InTabID, TabButtonEntryWidgetClass, nullptr);
    
	// 通过Tab ID查找刚创建的Tab按钮实例
	// GetTabButtonBaseByID是父类提供的函数，返回UCommonButtonBase*（所有Tab按钮的基类）
	if (UFrontendCommonButtonBase* FoundButton = Cast<UFrontendCommonButtonBase>(GetTabButtonBaseByID(InTabID)))
	{
		// 设置按钮上显示的文字
		// UFrontendCommonButtonBase是项目自定义的按钮类，提供了SetButtonText接口来修改显示文本
		FoundButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR	
void UFrontendTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(
			FString::Printf(TEXT("变量 'TabButtonEntryWidgetClass' 未指定有效的按钮类。请在蓝图 '%s' 中为其分配一个继承自 UFrontendCommonButtonBase 的有效 Widget 类。"),
				*GetClass()->GetName())
		));
	}
}
#endif