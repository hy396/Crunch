// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "FrontendUI/FrontendTypes/FrontendEnumTypes.h"
#include "GameplayTagContainer.h" 
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendUISubsystem.generated.h"

class UWidget_ActivatableBase;
class UWidget_PrimaryLayout;
class UFrontendCommonButtonBase;

// 异步推送小部件过程中的状态枚举
enum class EAsyncPushWidgetState:uint8
{
	// 小部件创建完成但在推送到栈之前的状态
	OnCreatedBeforePush,
	// 小部件推送到栈之后的状态
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, UFrontendCommonButtonBase*, BroadcastingButton, FText, DescriptionText );
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

	/**
	 * 异步推送软部件到指定的UI堆栈
	 * 此函数会异步加载部件类并将其添加到指定的UI堆栈中
	 * @param InWidgetStackTag 要推送部件到的UI堆栈标签
	 * @param InSoftWidgetClass 要添加的软部件类的软引用
	 * @param AysncPushStateCallback 异步推送状态回调函数，会在部件创建前后被调用
	 */
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AysncPushStateCallback);

	/**
	 * 异步将确认屏幕推送到模态堆栈
	 * 根据传入的确认屏幕类型创建对应的确认屏幕信息对象，并将其推送到模态UI堆栈
	 * @param InScreenType 确认屏幕类型（OK、YesNo、OkCancel等）
	 * @param InScreenTitle 弹窗标题文本
	 * @param InScreenMessage 弹窗正文说明文本
	 * @param ButtonClickedCallback 按钮点击后的回调函数
	 */	
	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);

	// 按钮描述文本更新
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;
	
	/**
	 * 设置主布局的显示状态
	 * @param bVisible 真则显示，假就隐藏
	 * @return 是否成功
	 */
	UFUNCTION(BlueprintCallable)
	bool SetPrimaryLayoutVisibility(bool bVisible);
private:
	// 属性是瞬态的，意味着它不会被保存或加载。被这样标记的属性在加载时零填充。
	UPROPERTY(Transient)    	// Transient  短暂的  Transient 是优化资源使用和确保数据一致性的重要工具，用于标记"不需要保存的运行时临时数据"，避免不必要的序列化和存储开销。
	UWidget_PrimaryLayout* CreatedPrimaryLayout;   // layout  布局
};
