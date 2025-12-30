// 幻雨喜欢小猫咪


#include "FrontendGameplayTags.h"

namespace FrontendGameplayTags
{	
	//Frontend widget stack
	namespace WidgetStack
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Modal, "Frontend.WidgetStack.Modal", "模态窗口层叠标签，用于管理模态对话框的层级") 
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameMenu, "Frontend.WidgetStack.GameMenu", "游戏菜单层叠标签，用于管理游戏内菜单的层级") 
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameHud, "Frontend.WidgetStack.GameHud", "游戏HUD层叠标签，用于管理游戏界面HUD的层级") 
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Frontend, "Frontend.WidgetStack.Frontend", "前端界面层叠标签，用于管理前端界面的层级")

	}
	//Frontend widgets
	namespace Widget
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PressAnyKeyScreen, "Frontend.Widget.PressAnyKeyScreen", "任意键继续界面标签，用于标识按任意键继续界面")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(MainMenuScreen, "Frontend.Widget.MainMenuScreen", "主菜单界面标签，用于标识游戏主菜单界面")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(OptionsScreen, "Frontend.Widget.ConfirmScreen", "确认界面标签，用于标识确认对话框界面")
			
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConfirmScreen, "Frontend.Widget.OptionsScreen", "选项设置界面标签，用于标识游戏选项设置界面")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KeyRemapScreen, "Frontend.Widget.KeyRemapScreen", "按键重映射界面标签，用于标识按键设置界面")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreditScreen, "Frontend.Widget.CreditScreen", "制作人员名单界面标签，用于标识游戏制作人员名单展示界面")
	}
	//Frontend Options Image
	namespace Image
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TestImage, "Frontend.Image.TestImage", "测试图片标签，用于标识选项界面中的测试图片资源")
	}
	// Frontend widgets

	// Frontend CreditScreen
	// UE_DEFINE_GAMEPLAY_TAG_COMMENT(Frontend_Widget_CreditScreen, "Frontend.Widget.CreditScreen", "制作人员名单界面标签，用于标识游戏制作人员名单展示界面")

}