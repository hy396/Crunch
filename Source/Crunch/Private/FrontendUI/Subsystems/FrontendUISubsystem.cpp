// 幻雨喜欢小猫咪


#include "FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "FrontendUI/FrontendFunctionLibrary.h"
#include "FrontendUI/Core/FrontendGameplayTags.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "FrontendUI/Widgets/Widget_ActivatableBase.h"
#include "FrontendUI/Widgets/Widget_ConfirmScreen.h"
#include "FrontendUI/Widgets/Widget_PrimaryLayout.h"

UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
	}
	return nullptr;
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 是否不是专用服务器实例
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);  // Derived 导出的；衍生的，派生的

		return FoundClasses.IsEmpty();  // 如果是空的返回true
	}
	return false;
}

void UFrontendUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget)
{
	// 检查传入的指针是否为空，如果为空则记录详细错误信息
	checkf(InCreatedWidget, TEXT("RegisterCreatedPrimaryLayoutWidget 被空小部件调用。这表明UI初始化序列存在问题。")); // 引发运行时错误，但仍然返回 nullptr，调用代码预期会优雅地处理这种情况。
	
	CreatedPrimaryLayout = InCreatedWidget;
	// Debug::Print(TEXT("成功存储主要布局小部件"));
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AysncPushStateCallback)
{
	// 检查要加载的小部件类引用是否为空
	check(!InSoftWidgetClass.IsNull());
	
	// 使用资产管理器的可流式加载管理器异步加载小部件类
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(InSoftWidgetClass.ToSoftObjectPath(),
		// 创建_lambda回调函数，在资源加载完成后执行
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, this, InWidgetStackTag, AysncPushStateCallback]()
			{
				// 获取已加载的小部件类
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
				// 确保小部件类已成功加载且主布局已创建
				check(LoadedWidgetClass && CreatedPrimaryLayout);

				// 根据标签查找对应的小部件栈容器
				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				
				// 将加载的小部件类添加到找到的小部件栈中
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass,
					// 添加小部件时的初始化回调函数，捕获这个TFunction，AysncPushStateCallback，再这个Lambda函数调用的时候调用这个输出
					[AysncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
					{   // 初始化小部件
						AysncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
					});
					
				// 推送后再次通知调用者，再次调用这个异步推送状态回调
				AysncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		)
	);
}

void UFrontendUISubsystem::PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType,
	const FText& InScreenTitle, const FText& InScreenMessage,
	TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
	UConfirmScreenInfoObject* CreatedInfoObject = nullptr;
	// 根据确认屏幕类型创建对应的确认屏幕信息对象
	switch (InScreenType)
	{
		case EConfirmScreenType::Ok:
			// 创建"确定"类型的确认屏幕
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::YesNo:
			// 创建"是/否"类型的确认屏幕
			CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::OkCancel:
			// 创建"确定/取消"类型的确认屏幕
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOKCancelScreen(InScreenTitle, InScreenMessage);
			break;
		case EConfirmScreenType::Unknown:
			// 未知类型，不创建任何对象
			break;
		default:
			// 默认情况，不创建任何对象
			break;
	}
	// 确保成功创建了确认屏幕信息对象
	check(CreatedInfoObject);

	// 异步将软部件推送到指定的UI堆栈
	// 使用模态堆栈标签和确认屏幕的软部件类标签，将确认屏幕添加到UI堆栈中
	PushSoftWidgetToStackAsync(FrontendGameplayTags::WidgetStack::Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Widget::ConfirmScreen),
		[CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			// 在小部件实例创建完成但尚未入栈时（OnCreatedBeforePush）进行初始化
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				// 将推送的部件转换为确认屏幕部件类型
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
				// 初始化确认屏幕，传入信息对象和按钮点击回调函数
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
			}
		}
	);
}

bool UFrontendUISubsystem::SetPrimaryLayoutVisibility(bool bVisible)
{
	if (CreatedPrimaryLayout)
	{
		CreatedPrimaryLayout->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		return true;
	}
	return false;
}

bool UFrontendUISubsystem::IsPrimaryLayoutRegistered() const
{
	return CreatedPrimaryLayout != nullptr;
}
