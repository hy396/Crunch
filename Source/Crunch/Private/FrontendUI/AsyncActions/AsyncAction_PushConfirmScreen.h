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
 * 异步推送确认屏幕的 Blueprint 操作节点
 *
 * 该类封装了一个标准的 Blueprint Async Action，用于：
 * - 显示模态确认屏幕
 * - 等待用户输入
 * - 将用户选择结果异步返回给 Blueprint
 *
 * 典型使用场景：
 * - 退出游戏确认
 * - 删除/覆盖数据确认
 * - 高风险操作提示
 *
 * 特点：
 * - 不阻塞游戏线程
 * - 生命周期由 GameInstance 管理
 * - 操作完成后自动销毁
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
	UFUNCTION(
		BlueprintCallable,
		meta = (
			WorldContext = "WorldContextObject",
			HidePin = "WorldContextObject",
			BlueprintInternalUseOnly="true",
			DisplayName="Show Confirmation Screen"
			),
		Category = "Async Tasks")
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
	/** 异步节点所属的世界（弱引用，避免生命周期问题） */
	TWeakObjectPtr<UWorld> CachedOwningWorld;

	/** 确认屏幕类型 */
	EConfirmScreenType CachedScreenType;

	/** 确认屏幕标题文本 */
	FText CachedScreenTitle;

	/** 确认屏幕正文文本 */
	FText CachedScreenMessage;
};
