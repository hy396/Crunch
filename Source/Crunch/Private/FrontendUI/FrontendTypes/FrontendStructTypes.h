// 幻雨喜欢小猫咪

#pragma once

#include "FrontendStructTypes.generated.h"

/**
 * 用于描述选项数据（Options Data）的编辑条件（Edit Condition）及相关行为。
 * 
 * 此结构体支持：
 * - 动态判断某个选项是否应处于可编辑状态（通过绑定函数）
 * - 提供禁用时的富文本提示理由（如“仅在开发者模式下可用”）
 * - 支持强制覆盖选项的字符串值（即使被禁用，也可显示/导出特定值）
 */
USTRUCT(BlueprintType)
struct FOptionsDataEditConditionDescriptor
{
	GENERATED_BODY()

public:
	/**
	 * 设置用于判断编辑条件是否满足的函数。
	 * 
	 * @param InEditConditionFunc 一个无参数、返回 bool 的可调用对象（如 Lambda 或函数指针）。
	 *                            返回 true 表示允许编辑，false 表示禁止编辑。
	 */
	void SetEditConditionFunc(TFunction<bool()> InEditConditionFunc)
	{
		EditConditionFunc = InEditConditionFunc;
	}

	/**
	 * 判断是否已设置有效的编辑条件函数。
	 * 
	 * @return 若已绑定函数则返回 true，否则返回 false。
	 */
	bool IsValid() const
	{
		return EditConditionFunc != nullptr;
	}

	/**
	 * 执行绑定的编辑条件函数，判断当前是否允许编辑。
	 * 
	 * @return 若条件函数存在且返回 true，则允许编辑；
	 *         若未设置条件函数，默认视为条件满足（返回 true）。
	 */
	bool IsEditConditionMet() const
	{
		if (IsValid())
		{
			return EditConditionFunc(); // 执行绑定的函数
		}
		return true; // 未设置条件时默认允许编辑
	}

	/**
	 * 获取选项被禁用时显示的富文本提示信息（例如说明原因）。
	 * 
	 * @return 禁用原因的富文本字符串（支持 Slate/UMG 富文本格式，如 <RichText> 标签）。
	 */
	FString GetDisabledRichReason() const
	{
		return DisabledRichReason;
	}

	/**
	 * 设置选项被禁用时的富文本提示信息。
	 * 
	 * @param InDisabledRichReason 富文本格式的禁用原因（例如 "仅限调试模式"）。
	 */
	void SetDisabledRichReason(const FString& InDisabledRichReason)
	{
		DisabledRichReason = InDisabledRichReason;
	}

	/**
	 * 判断是否设置了强制字符串值（即使选项被禁用，仍可使用此值进行显示或导出）。
	 * 
	 * @return 若已设置强制字符串值则返回 true，否则返回 false。
	 */
	bool HasForcedStringValue() const
	{
		return DisabledForcedStringValue.IsSet();
	}

	/**
	 * 获取强制指定的字符串值（通常用于被禁用选项的回退值或导出值）。
	 * 
	 * @return 强制字符串值；若未设置，调用此函数前应先检查 HasForcedStringValue()。
	 */
	FString GetDisabledForcedStringValue() const
	{
		return DisabledForcedStringValue.GetValue();
	}

	/**
	 * 设置当选项被禁用时,应使用的强制字符串值。
	 * 
	 * @param InDisabledForcedStringValue 要强制使用的字符串值（例如 "N/A" 或默认配置值）。
	 */
	void SetDisabledForcedStringValue(const FString& InDisabledForcedStringValue)
	{
		DisabledForcedStringValue = InDisabledForcedStringValue;
	}

private:
	/** 存储一个无参数、返回 bool 的可调用对象，用于动态判断编辑权限。 */
	TFunction<bool()> EditConditionFunc;

	/** 当编辑条件不满足时，向用户显示的富文本格式的禁用原因。 */
	FString DisabledRichReason;

	/** 可选的强制字符串值，用于在选项被禁用时替代实际值（例如用于配置导出或显示）。 */
	TOptional<FString> DisabledForcedStringValue;
};