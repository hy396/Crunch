// 幻雨喜欢小猫咪


#include "AsyncAction_PushSoftWidget.h"
#include "FrontendUI/Subsystems/FrontendUISubsystem.h"
#include "FrontendUI/Widgets/Widget_ActivatableBase.h"

/**
 * PushSoftWidget
 *
 * 创建一个用于异步加载并推送软类引用小部件的 Blueprint 异步操作实例。
 *
 * 该函数本身不会立即执行任何 UI 逻辑，仅负责：
 * - 校验参数合法性
 * - 缓存必要的上下文数据
 * - 创建并注册异步操作对象
 *
 * 实际的异步加载与小部件推送逻辑将在 Activate() 中启动。
 *
 * @param WorldContextObject 世界上下文对象，用于解析 UWorld
 * @param OwningPlayerController 小部件的 Owning Player，用于输入与焦点归属
 * @param InSoftWidgetClass 要异步加载并推送的软类引用小部件类型
 * @param InWidgetStackTag 目标 Widget Stack 的标识标签
 * @param bFocusOnNewlyPushedWidget 推入完成后是否自动设置焦点
 *
 * @return 创建成功的异步操作实例；若 World 无法解析则返回 nullptr
 */
UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(
	const UObject* WorldContextObject,
	APlayerController* OwningPlayerController,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	UPARAM(meta = (Categories = "Frontend.WidgetStack"))
	FGameplayTag InWidgetStackTag,
	bool bFocusOnNewlyPushedWidget
)
{
	// 软类引用不能为空，该错误通常表示调用方逻辑存在问题
	checkf(!InSoftWidgetClass.IsNull(), TEXT("调用 PushSoftWidgetToStack 时传入的软类小部件引用为空"));

	if (GEngine)
	{
		// 通过世界上下文对象解析当前 UWorld
		if (UWorld* World = GEngine->GetWorldFromContextObject(
			WorldContextObject,
			EGetWorldErrorMode::LogAndReturnNull))//引发运行时错误，但仍然返回 nullptr，调用代码预期会优雅地处理这种情况。
		{
			// 创建异步操作对象（非立即执行）
			UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();

			// 缓存执行所需的上下文数据
			Node->CachedOwningWorld = World;
			Node->CachedOwingPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

			// 注册到 GameInstance，确保异步执行期间不会被 GC 回收
			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}

	return nullptr;
}

/**
 * Activate
 *
 * 激活异步操作，由引擎在 Blueprint 节点执行时自动调用。
 *
 * 该函数会：
 * - 获取前端 UI 子系统
 * - 请求其异步加载软类引用的小部件
 * - 在不同生命周期阶段通过回调通知外部逻辑
 *
 * 注意：
 * - 小部件的创建与推入栈操作仍然发生在 Game Thread
 * - 异步仅体现在软类引用资源的加载阶段
 */
void UAsyncAction_PushSoftWidget::Activate()
{
	// 获取前端 UI 子系统实例
	UFrontendUISubsystem* FrontendUISubsystem =
		UFrontendUISubsystem::Get(CachedOwningWorld.Get());

	// 请求异步加载并推送软类引用小部件
	FrontendUISubsystem->PushSoftWidgetToStackAsync(
		// 目标 Widget Stack
		CachedWidgetStackTag,
		// 要加载的小部件软类引用
		CachedSoftWidgetClass,
		// 推送过程中的生命周期回调
		[this](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			switch (InPushState)
			{
			case EAsyncPushWidgetState::OnCreatedBeforePush:
				{
					// 小部件已创建但尚未推入栈
					// 在此阶段设置 Owning Player，确保输入与焦点系统正常工作
					PushedWidget->SetOwningPlayer(CachedOwingPC.Get());

					// 通知外部逻辑进行初始化（数据填充、事件绑定等）
					OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
				}
				break;

			case EAsyncPushWidgetState::AfterPush:
				{
					// 小部件已成功推入 Widget Stack
					AfterPush.Broadcast(PushedWidget);

					// 根据配置决定是否自动设置焦点
					if (bCachedFocusOnNewlyPushedWidget)
					{
						// 获取小部件期望获得焦点的目标控件
						if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
						{
							WidgetToFocus->SetFocus();
						}
					}

					// 标记异步操作完成，允许引擎回收该节点
					SetReadyToDestroy();
				}
				break;

			default:
				break;
			}
		}
	);
}
