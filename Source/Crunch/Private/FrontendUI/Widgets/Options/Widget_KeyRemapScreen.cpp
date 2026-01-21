// 幻雨喜欢小猫咪


#include "Widget_KeyRemapScreen.h"

#include "CommonInputSubsystem.h"
#include "CommonRichTextBlock.h"
#include "Framework/Application/IInputProcessor.h"
#include "ICommonInputModule.h"
#include "CommonUITypes.h"

/**
 * Slate 输入预处理器 (Input Preprocessor)
 *
 * 作用：
 * 这是一个极低层级的输入钩子，它位于 Enhanced Input 和 UMG 之前。
 * 它允许我们在按键事件分发给任何 Widget 之前拦截它。
 * 
 * 职责：
 * 1. 监听所有输入。
 * 2. 判断输入是否符合当前的改键规则（如：是否是 ESC，是否设备不匹配）。
 * 3. 返回 true 以“消费”输入，确保改键时不会触发游戏内的跳跃或开火。
 */
class FKeyRemapScreenInputPreprocessor : public IInputProcessor
{
public:
	/**
	 * 构造函数
	 * @param InInputTypeToListenTo 期望监听的输入类型（键鼠 / 手柄）
	 * @param InOwningLocalPlayer   当前 Widget 所属的本地玩家，用于获取 CommonInputSubsystem
	 */
	FKeyRemapScreenInputPreprocessor(ECommonInputType InInputTypeToListenTo, ULocalPlayer* InOwningLocalPlayer)
	: CachedInputTypeToListenTo(InInputTypeToListenTo), CachedWeakOwningLocalPlayer(InOwningLocalPlayer)
	{
	}

	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeyPressedDelegate, const FKey& /*PressedKey*/)
	FOnInputPreProcessorKeyPressedDelegate OnInputPreProcessorKeyPressed;

	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeySelectCanceledDelegate, const FString& /*CanceledReason*/)
	FOnInputPreProcessorKeySelectCanceledDelegate OnInputPreProcessorKeySelectCanceled;

protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		// 不需要每帧处理，留空
	}

	// 键盘按键按下事件（Slate 层最早回调）
	virtual bool HandleKeyDownEvent(
		FSlateApplication& SlateApp,
		const FKeyEvent& InKeyEvent
	) override
	{
		// 处理按键逻辑
		ProcessPressedKey(InKeyEvent.GetKey());

		// 返回 true 表示“消费输入”(Consume)，阻止事件继续向下传播给其他 UI 或游戏逻辑
		return true;
	}

	// 鼠标按键按下事件
	virtual bool HandleMouseButtonDownEvent(
		FSlateApplication& SlateApp,
		const FPointerEvent& MouseEvent
	) override
	{
		// 处理鼠标按键逻辑
		ProcessPressedKey(MouseEvent.GetEffectingButton());

		// 同样消费输入
		return true;
	}

	/**
	 * 核心逻辑：处理捕获到的按键
	 *
	 * 校验规则：
	 * 1. Esc 键：视为“取消改键”。
	 * 2. 设备匹配检查：
	 *    - 如果期望键鼠，却按了手柄 -> 报错取消。
	 *    - 如果期望手柄，却按了键盘 -> 报错取消。
	 * 3. 特殊情况：
	 *    - 在手柄模式下，鼠标左键点击 UI 通常会被识别为手柄的“确认”键（Virtual Accept）。
	 *    - 这里需要手动查表，将鼠标左键转换为对应的手柄确认键（如 Xbox 的 A 键）。
	 */
	void ProcessPressedKey(const FKey& InPressedKey) const
	{
		// 🛑 规则1: 禁止映射 Esc (用于退出 UI)
		if (InPressedKey == EKeys::Escape)
		{
			// ReSharper disable once CppExpressionWithoutSideEffects
			OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("Esc键保留用于退出，不可映射"));
			return;
		}

		// 获取 CommonInputSubsystem，用于判断当前系统实际的输入源状态
		UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedWeakOwningLocalPlayer.Get());
		check(CommonInputSubsystem);
		ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
		
		switch (CachedInputTypeToListenTo)
		{
		case ECommonInputType::MouseAndKeyboard:
			// 键鼠模式下：如果按下了手柄键，或者当前被系统判定为手柄输入，则取消
			if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("检测到手柄输入，请按下键盘或鼠标按键"));
				return;
			}
			break;
		case ECommonInputType::Gamepad:
			// 手柄模式特殊处理：鼠标左键点击
			// 如果是用鼠标点击了改键按钮，或者系统认为是手柄模式但检测到左键
			if (CurrentInputType == ECommonInputType::Gamepad && InPressedKey == EKeys::LeftMouseButton)
			{
				// 获取 CommonUI 设置中的“默认点击确认行为”对应的数据
				// 目的：将鼠标点击转换为手柄的 FaceButton_Bottom (A/Cross)
				if(const FCommonInputActionDataBase* InputActionData = ICommonInputModule::GetSettings().GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT("CommonUI配置缺失")))
				{
					// ReSharper disable once CppExpressionWithoutSideEffects
					OnInputPreProcessorKeyPressed.ExecuteIfBound(InputActionData->GetDefaultGamepadInputTypeInfo().GetKey());
				}
				else
				{
					checkf(false, TEXT("无法获取CommonUI默认点击行为配置"));
				}
				return;
			}
			
			// 🛑 规则2: 设备类型匹配 (防止键鼠模式下录入手柄键)
			if (!InPressedKey.IsGamepadKey())
			{
				// ReSharper disable once CppExpressionWithoutSideEffects
				OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("检测到键盘输入，请按下手柄按键"));
				return;
			}
			break;

		default:
			break;
		}

		// ✅ 验证通过，广播按键
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnInputPreProcessorKeyPressed.ExecuteIfBound(InPressedKey);
	}

private:
	ECommonInputType CachedInputTypeToListenTo;
	TWeakObjectPtr<ULocalPlayer> CachedWeakOwningLocalPlayer;
};

// --------------------------------------------------------
// UWidget_KeyRemapScreen Implementation
// --------------------------------------------------------

void UWidget_KeyRemapScreen::SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType)
{
	CachedDesiredInputType = InDesiredInputType;
}

void UWidget_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	// 1. 创建输入预处理器实例
	CachedInputPreprocessor = MakeShared<FKeyRemapScreenInputPreprocessor>(CachedDesiredInputType, GetOwningLocalPlayer());
	
	// 2. 绑定处理器的回调到本 Widget 的函数
	CachedInputPreprocessor->OnInputPreProcessorKeyPressed.BindUObject(this, &ThisClass::OnValidKeyPressedDetected);
	CachedInputPreprocessor->OnInputPreProcessorKeySelectCanceled.BindUObject(this, &ThisClass::OnKeySelectedCanceled);
	
	// 3. 注册到 Slate Application
	// 索引 -1 表示尽可能早地处理（但在特定的高优先级处理器之后）
	FSlateApplication::Get().RegisterInputPreProcessor(CachedInputPreprocessor, -1);

	// 4. 更新 UI 提示文本
	FString InputDeviceName;
	switch (CachedDesiredInputType)
	{
		case ECommonInputType::MouseAndKeyboard:
			// InputDeviceName = TEXT("Mouse & Keyboard");
			InputDeviceName = TEXT("键盘||鼠标"); // 对应中文
			break;
		case ECommonInputType::Gamepad:
			// InputDeviceName = TEXT("Gamepad");
			InputDeviceName = TEXT("手柄"); // 对应中文
			break;
		default:
			break;
	}
	
	// 使用富文本格式化提示语
	// 原文： "<KeyRemapDefault>Press any</> <KeyRemapHighlight>%s</> <KeyRemapDefault>key.</>"
	// 译文保留标签结构：
	const FString DisplayRichMessage = FString::Printf(
		TEXT("<KeyRemapDefault>请按下任意</> <KeyRemapHighlight>%s</> <KeyRemapDefault>按键</>"), 
		*InputDeviceName
	);
	CommonRichText_RemapMessage->SetText(FText::FromString(DisplayRichMessage));
}

void UWidget_KeyRemapScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	// 必须在 Widget 关闭时注销处理器，否则会一直拦截输入导致游戏无法操作
	if (CachedInputPreprocessor)
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(CachedInputPreprocessor);
		CachedInputPreprocessor.Reset();
	}
}

void UWidget_KeyRemapScreen::OnValidKeyPressedDetected(const FKey& PressedKey)
{
	// 捕获成功，请求延迟关闭并传出按键数据
	RequestDeactivateWidget([this, PressedKey]()
	{
		// Debug::Print(TEXT("被按下的键： ") + PressedKey.GetDisplayName().ToString());
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnKeyRemapScreenKeyPressed.ExecuteIfBound(PressedKey);
	});
}

void UWidget_KeyRemapScreen::OnKeySelectedCanceled(const FString& CanceledReason)
{
	// 捕获失败或取消，请求延迟关闭并传出原因
	RequestDeactivateWidget([this, CanceledReason]()
	{
		// Debug::Print(CanceledReason);
		// ReSharper disable once CppExpressionWithoutSideEffects
		OnKeyRemapScreenKeySelectCanceled.ExecuteIfBound(CanceledReason);
	});
}

void UWidget_KeyRemapScreen::RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback)
{
	// 使用 CoreTicker 延迟一帧执行关闭逻辑
	// 原因：输入事件还在传递中，立即销毁 Widget 可能导致 Crash 或输入状态错误
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda([this, PreDeactivateCallback](float DeltaTime)->bool
		{
			// 1. 执行回调（更新数据）
			PreDeactivateCallback();
			// 2. 关闭 Widget（CommonUI 逻辑）
			DeactivateWidget();
			
			// 返回 false 表示该 Ticker 只运行一次，随即销毁
			return false;
		}));
}
