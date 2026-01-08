// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_OptionsDetailsView.generated.h"

class UCommonRichTextBlock;   // 富文本控件（支持加粗、颜色等）
class UCommonTextBlock;       // 普通文本控件
class UCommonLazyImage;       // 懒加载图片控件（支持软对象引用）
class UListDataObject_Base;   // 选项列表数据对象基类

/**
 * 选项详细信息面板控件
 * 
 * 位于选项界面右侧，用于显示当前选中（或悬停）选项的详细说明，包括：
 * - 选项标题
 * - 描述图片（可选）
 * - 富文本描述
 * - 动态调试信息（数据对象类名 + 条目控件类名）
 * - 当前不可编辑时的禁用原因（如“需要重启游戏生效”）
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_OptionsDetailsView : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * 更新详细信息面板内容
	 * @param InDataObject           当前选中的选项数据对象
	 * @param InEntryWidgetClassName 对应的列表条目控件类名（用于调试显示）
	 */
	void UpdateDetailsViewInfo(UListDataObject_Base* InDataObject, const FString& InEntryWidgetClassName = FString());

	/** 清空所有详细信息，恢复到初始空白状态 */
	void ClearDetailsViewInfo();

protected:
	// ==================== UUserWidget 接口 ====================
	// Widget 初始化完成时调用
	virtual void NativeOnInitialized() override;
	// =========================================================

private:
	// ====================== 绑定的子控件 ======================
	// 选项标题文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Title;

	// 描述图片（懒加载，支持软对象引用，避免打包时硬加载）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> CommonLazyImage_DescriptionImage;

	// 静态富文本描述（来自数据对象配置的描述内容）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_Description;

	// 动态生成的调试信息（数据类名 + 条目控件类名）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_DynamicDetails;

	// 当前选项不可编辑时的原因说明（如“需要在主菜单修改”或“重启后生效”）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_DisabledReason;
	// ===========================================================
};
