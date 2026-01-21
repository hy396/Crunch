// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "FrontendUI/Widgets/Widget_ActivatableBase.h"
#include "Widget_KeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FKeyRemapScreenInputPreprocessor;

/**
 * 按键重映射界面 Widget
 *
 * 该界面在激活时会：
 * - 注册一个 Slate Input PreProcessor
 * - 捕获玩家的下一次输入（键盘 / 鼠标 / 手柄）
 * - 根据期望的输入设备类型进行过滤
 * - 成功则返回捕获到的 FKey
 * - 失败或取消（Esc / 非法输入）则返回取消原因
 *
 * 注意：
 * - 输入捕获发生在 Slate 层，而非 Enhanced Input
 * - Widget 激活期间会“吞掉”输入
 * - 捕获完成后会延迟一帧再关闭自身，避免输入丢失
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class CRUNCH_API UWidget_KeyRemapScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
public:
	/** 
	 * 设置当前重映射界面期望监听的输入设备类型
	 * @param InDesiredInputType - ECommonInputType::MouseAndKeyboard 或 Gamepad
	 * 用于过滤玩家的误触（例如：在改手柄键时按了键盘，会被拒绝）
	 */
	void SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType);

	/** 
	 * 委托：成功捕获到合法按键时触发 
	 * @param PressedKey - 捕获到的 FKey
	 */
	DECLARE_DELEGATE_OneParam(
		FOnKeyRemapScreenKeyPressedDelegate,
		const FKey& /*PressedKey*/
	)
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;

	/** 
	 * 委托：捕获流程被取消时触发 
	 * 触发情况：按下 Esc 键、输入了错误的设备类型按键等
	 * @param CanceledReason - 取消的原因描述
	 */
	DECLARE_DELEGATE_OneParam(
		FOnKeyRemapScreenKeySelectCanceledDelegate,
		const FString& /*CanceledReason*/
	)
	FOnKeyRemapScreenKeySelectCanceledDelegate OnKeyRemapScreenKeySelectCanceled;
	
protected:
	// begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;  // widget创建激活时
	virtual void NativeOnDeactivated() override; //widget关闭时
	// end UCommonActivatableWidget Interface

private:
	// ****需要绑定的组件
	/** 显示提示信息的富文本组件（例如："Press any Key..."） */
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* CommonRichText_RemapMessage;  // 用来显示消息
	// ****需要绑定的组件

	/** 自定义的 Slate 输入预处理器指针 */
	TSharedPtr<FKeyRemapScreenInputPreprocessor> CachedInputPreprocessor;
	
	/** 缓存的期望输入类型 */
	ECommonInputType CachedDesiredInputType;

	/** 内部回调：当处理器检测到有效按键时 */
	void OnValidKeyPressedDetected(const FKey& PressedKey);  // Detected 检测到的

	/** 内部回调：当处理器决定取消操作时 */
	void OnKeySelectedCanceled(const FString& CanceledReason);

	// 延迟检查以确保输入键正确捕获， 在调用PreDeactivateCallback激活回调之前，并禁用重置
	/**
	 * 请求关闭 Widget（延迟一帧）
	 *
	 * 原因：
	 * - 输入事件仍在 Slate / Input 栈中传播
	 * - 立刻关闭 Widget 可能导致：
	 *   - 输入丢失
	 *   - 焦点状态异常
	 *
	 * 因此使用 CoreTicker 延迟到下一帧执行：
	 * - 先触发回调
	 * - 再 Deactivate Widget
	 */
	void RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback);
};
