// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_PushSoftWidget.generated.h"

class UWidget_ActivatableBase;

/**
 * 异步推送软类引用小部件到指定小部件栈的委托
 *
 * @param PushedWidget 已创建的小部件实例指针。
 *        注意：在 OnWidgetCreatedBeforePush 阶段，该小部件尚未被推入栈；
 *              在 AfterPush 阶段，该小部件已成功推入栈。
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPushSoftWidgetDelegate,
	UWidget_ActivatableBase*,
	PushedWidget
);

/**
 * UAsyncAction_PushSoftWidget
 *
 * 用于将软类引用（TSoftClassPtr）的小部件异步加载并推送到指定小部件栈的 Blueprint 异步操作类。
 *
 * 该类基于 BlueprintAsyncActionBase 实现，通过异步加载软引用的小部件类资源，
 * 在资源加载完成后创建小部件实例，并将其推送至目标 Widget Stack。
 *
 * 软类引用（Soft Class Reference）仅在实际使用时才会触发资源加载，
 * 可有效降低 UI 初始加载时间和首次打开界面时的卡顿风险。
 *
 * 主要用途：
 * 1. 使用软类引用延迟加载 UI 小部件资源，减少启动阶段的内存占用
 * 2. 在蓝图中以异步方式加载 UI 类资源，避免同步加载导致的帧卡顿
 * 3. 在小部件创建与入栈两个生命周期阶段提供回调，便于插入初始化与展示逻辑
 *
 * 执行流程：
 * 1. 调用 PushSoftWidget 创建异步操作实例
 * 2. 引擎异步加载软引用的小部件类资源
 * 3. 资源加载完成后创建小部件实例
 * 4. 触发 OnWidgetCreatedBeforePush（小部件已创建，但尚未推入栈）
 * 5. 通过 UI 子系统将小部件推入指定的 Widget Stack
 * 6. 触发 AfterPush（小部件已成功推入栈）
 *
 * 蓝图中的典型用法：
 * - 使用 “Push Soft Widget To Widget Stack” 节点
 * - 在 OnWidgetCreatedBeforePush 中进行数据初始化、事件绑定等操作
 * - 在 AfterPush 中执行动画播放、音效触发或焦点处理等展示逻辑
 *
 * 生命周期说明：
 * - 异步操作对象在委托触发完成前会被引擎自动持有引用，不会被 GC 回收
 * - 小部件的实际创建与推入栈操作仍发生在 Game Thread
 */
UCLASS()
class CRUNCH_API UAsyncAction_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// Internal  内部的  BlueprintInternalUseOnly="true" "此元素仅在蓝图内部逻辑中可用，不允许在蓝图图表中作为公开节点调用/访问。"
	/**
	 * 异步推送软类引用小部件到指定小部件栈的内部静态方法
	 *
	 * @param WorldContextObject 世界上下文对象，用于获取 World 引用
	 * @param OwningPlayerController 小部件所属的玩家控制器
	 * @param InSoftWidgetClass 要加载并推送的软类引用小部件类型
	 * @param InWidgetStackTag 目标 Widget Stack 的标识标签，
	 *        通常属于 "Frontend.WidgetStack" 分类
	 * @param bFocusOnNewlyPushedWidget 推入后是否自动将焦点设置到新小部件
	 *
	 * @return 创建的异步操作实例
	 *
	 * 注意：
	 * - 该函数被标记为 BlueprintInternalUseOnly，
	 *   不会作为普通蓝图节点出现在右键菜单中
	 * - 通常由自定义 K2Node 或包装用的 Blueprint 函数间接调用
	 */
	UFUNCTION(
		BlueprintCallable,
		meta = (
			WorldContext = "WorldContextObject",
			HidePin = "WorldContextObject",
			BlueprintInternalUseOnly = "true",
			DisplayName = "Push Soft Widget To Widget Stack"
		),
		Category = "Async Tasks"
	)
	static UAsyncAction_PushSoftWidget* PushSoftWidget(const UObject* WorldContextObject, APlayerController* OwningPlayerController,
		TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, UPARAM(meta=(Categories="Frontend.WidgetStack")) FGameplayTag InWidgetStackTag, bool bFocusOnNewlyPushedWidget = true);

	/**
	 * 激活异步操作
	 *
	 * 由引擎自动调用，用于启动软类引用的异步加载流程。
	 * 不应由用户手动调用。
	 */
	// ~开始 UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	// ~结束 UBlueprintAsyncActionBase Interface
	
	/**
	 * 小部件已创建但尚未推入栈时触发
	 *
	 * 适合进行小部件的数据初始化、事件绑定等操作。
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnWidgetCreatedBeforePush;

	/**
	 * 小部件已成功推入栈后触发
	 *
	 * 适合执行界面展示相关逻辑，如动画、音效或焦点设置。
	 */
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate AfterPush;

private:
	/** 缓存的 World 引用，用于获取 UI 子系统 */
	TWeakObjectPtr<UWorld> CachedOwningWorld;

	/** 缓存的玩家控制器，用于作为小部件的 Owning Player */
	TWeakObjectPtr<APlayerController> CachedOwingPC;

	/** 要异步加载的软类引用小部件类型 */
	TSoftClassPtr<UWidget_ActivatableBase> CachedSoftWidgetClass;

	/** 目标 Widget Stack 的标识标签 */
	FGameplayTag CachedWidgetStackTag;

	/** 推入后是否自动聚焦新创建的小部件 */
	bool bCachedFocusOnNewlyPushedWidget = false;
};
