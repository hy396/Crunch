// 幻雨喜欢小猫咪

#pragma once

UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
	Ok UMETA(DisplayName = "确定"),
	YesNo UMETA(DisplayName = "是/否"),
	OkCancel UMETA(DisplayName = "确定/取消"),
	Unknown UMETA(Hidden)  // 引擎中看不到该项
};

UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
	Confirmed UMETA(DisplayName = "确认"),
	Cancelled UMETA(DisplayName = "取消"),
	Closed UMETA(DisplayName = "关闭"),
	Unknown UMETA(Hidden)  // 引擎中看不到该项
};

// tab标签下旋转器改变原因
UENUM(BlueprintType)
enum class EOptionsListDataModifyReason:uint8
{
	DirectlyModified UMETA(DisplayName = "直接修改"),  // 直接修改
	DependencyModified UMETA(DisplayName = "依赖项变更"),  // 依赖项变更
	ResetToDefault UMETA(DisplayName = "重置为默认值")  // 重置为默认值
};