// 幻雨喜欢小猫咪


#include "AsyncAction_PushConfirmScreen.h"
#include "FrontendUI/Subsystems/FrontendUISubsystem.h"

UAsyncAction_PushConfirmScreen* UAsyncAction_PushConfirmScreen::PushConfirmScreen(const UObject* WorldContextObject,
	EConfirmScreenType ScreenType, FText InScreenTitle, FText InScreenMessage)
{
	if (GEngine)
	{
		// 通过上下文对象获取当前世界引用
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			// 创建新的异步操作节点
			UAsyncAction_PushConfirmScreen* Node = NewObject<UAsyncAction_PushConfirmScreen>();

			// 缓存参数供后续使用
			Node->CachedOwningWorld = World;
			Node->CachedScreenType = ScreenType;
			Node->CachedScreenTitle = InScreenTitle;
			Node->CachedScreenMessage = InScreenMessage;

			// 注册到游戏实例以确保正确生命周期管理
			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}
	return nullptr;
}

/**
 * 激活异步操作，开始执行确认屏幕推送逻辑
 * 
 * 此方法会被引擎自动调用，不需要手动执行。
 * 它会请求前端UI子系统推送确认屏幕到模态栈，并注册回调函数处理用户响应。
 */
void UAsyncAction_PushConfirmScreen::Activate()
{
	// 获取前端UI子系统实例
	UFrontendUISubsystem::Get(CachedOwningWorld.Get())->PushConfirmScreenToModalStackAsync(
		CachedScreenType,
		CachedScreenTitle,
		CachedScreenMessage,
		// Lambda表达式定义回调函数，处理用户点击按钮后的逻辑
		[this](EConfirmScreenButtonType ClickedButtonType)
		{
			// 广播按钮点击事件，通知所有监听者
			OnButtonClicked.Broadcast(ClickedButtonType);
			// 设置为准备销毁状态，异步操作完成
			SetReadyToDestroy();
		}
	);
}
