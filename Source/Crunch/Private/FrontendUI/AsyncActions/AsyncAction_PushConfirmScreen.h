// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "FrontendUI/FrontendTypes/FrontendEnumTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_PushConfirmScreen.generated.h"

/**
 * 确认屏幕按钮点击事件委托
 * 当用户点击确认屏幕上的按钮时触发此委托
 * @param ClickedButtonType 被点击的按钮类型（确认/取消）
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClickedDelegate, EConfirmScreenButtonType, ClickedButtonType);

/**
 * 异步推送确认屏幕操作类
 * 
 * 该类提供了一种异步方式来显示确认屏幕，并在用户做出选择时通知调用者。
 * 可用于需要用户确认才能继续的操作，例如退出游戏、删除数据等场景。
 * 
 * 主要特性：
 * 1. 异步操作模式，不会阻塞游戏主线程
 * 2. 支持不同类型的确认屏幕（信息、警告、错误等）
 * 3. 提供按钮点击事件回调机制
 * 4. 自动管理生命周期，在操作完成后销毁
 */
UCLASS()
class CRUNCH_API UAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * 推送确认屏幕的静态工厂方法
	 * 
	 * @param WorldContextObject 世界上下文对象，用于获取当前世界
	 * @param ScreenType 确认屏幕类型（决定屏幕样式和图标）
	 * @param InScreenTitle 屏幕标题文本
	 * @param InScreenMessage 屏幕消息内容文本
	 * @return 异步操作实例，可用于绑定事件和监听结果；如果无法获取有效世界则返回nullptr
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly="true",DisplayName="Show Confirmation Screen"), Category = "Async Tasks")
	static UAsyncAction_PushConfirmScreen* PushConfirmScreen(const UObject* WorldContextObject,
		EConfirmScreenType ScreenType, FText InScreenTitle, FText InScreenMessage);

	// ~开始 UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	// ~结束 UBlueprintAsyncActionBase Interface

	/**
	 * 按钮点击事件委托
	 * 当用户点击确认屏幕上的任意按钮时广播此事件
	 */
	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClickedDelegate OnButtonClicked;
	
private:
	/** 缓存的世界引用 */
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	
	/** 缓存的确认屏幕类型 */
	EConfirmScreenType CachedScreenType;
	
	/** 缓存的屏幕标题 */
	FText CachedScreenTitle;
	
	/** 缓存的屏幕消息内容 */
	FText CachedScreenMessage;
};
