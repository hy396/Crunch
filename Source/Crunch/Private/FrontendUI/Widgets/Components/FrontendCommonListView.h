// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "FrontendCommonListView.generated.h"

class UDataAsset_DataListEntryMapping;   // 数据对象到列表条目控件的映射表

/**
 * 前端通用选项列表视图控件
 *
 * 继承自 CommonUI 的 UCommonListView，专为游戏选项界面设计。
 * 主要增强功能：
 * 1. 自动根据数据对象类型动态生成对应的列表条目 Widget（无需手动指定 Entry Class）
 * 2. 禁止将“Tab 集合”类型的数据项设为可选中/导航焦点（因为它们只是分组标题）
 * 3. 编辑器下强制检查映射表是否配置，避免运行时出错
 */
UCLASS()
class CRUNCH_API UFrontendCommonListView : public UCommonListView
{
	GENERATED_BODY()

protected:
	// ==================== UCommonListView 接口重载 ====================
	/**
	 * 内部生成列表条目 Widget 的核心函数
	 * 我们在这里拦截并实现“根据数据对象类型自动选择正确 Entry Widget”的逻辑
	 */
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, 
		TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

	/**
	 * 判断某个列表项是否允许被选中或通过手柄/键盘导航聚焦
	 * 用于屏蔽“Tab 集合”类型的项（它们仅作标题使用，不应获得焦点）
	 */
	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;
	// ===================================================================

private:
	// ==================== UWidget 编辑器接口 ====================
#if WITH_EDITOR
	/** 编辑器下编译蓝图时进行默认值校验（确保关键资源已配置） */
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	// ===========================================================

	/**
	 * 数据对象 ↔ 列表条目控件 的映射资源
	 * 在编辑器中配置一个 UDataAsset_DataListEntryMapping 实例
	 * 运行时通过它动态决定每种选项数据应该用哪个 UI 控件显示
	 */
	UPROPERTY(EditAnywhere, Category = "Frontend List View Settings",
		meta = (DisplayName = "数据到条目控件映射表"))
	UDataAsset_DataListEntryMapping* DataListEntryMapping;
};