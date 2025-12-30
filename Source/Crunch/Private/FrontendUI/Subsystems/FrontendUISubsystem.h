// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendUISubsystem.generated.h"

class UWidget_ActivatableBase;
class UWidget_PrimaryLayout;
struct FGameplayTag;
class UFrontendCommonButtonBase;

// 异步推送小部件过程中的状态枚举
enum class EAsyncPushWidgetState:uint8
{
	// 小部件创建完成但在推送到栈之前的状态
	OnCreatedBeforePush,
	// 小部件推送到栈之后的状态
	AfterPush
};

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, UFrontendCommonButtonBase*, BroadcastingButton, FText, DescriptionText );
/**
 * 为了将创建的小部件存储在子系统中
 */
UCLASS()
class CRUNCH_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
		
public:
	static UFrontendUISubsystem* Get(const UObject* WorldContextObject);

	//~ Begin Subsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End Subsystem Interface

	// 在前端控制器中调用了
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget);

	// 创建软小部件并将其推送到指定标签的栈中
	// @param InWidgetStackTag 用于标识目标小部件栈的标签
	// @param InSoftWidgetClass 要异步加载和推送的小部件类的软引用
	// @param AysncPushStateCallback 异步推送过程中的状态回调函数，在小部件创建前和推送后分别调用
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AysncPushStateCallback);

//	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);


private:
	// 属性是瞬态的，意味着它不会被保存或加载。被这样标记的属性在加载时零填充。
	UPROPERTY(Transient)    	// Transient  短暂的  Transient 是优化资源使用和确保数据一致性的重要工具，用于标记"不需要保存的运行时临时数据"，避免不必要的序列化和存储开销。
	UWidget_PrimaryLayout* CreatedPrimaryLayout;   // layout  布局
};
