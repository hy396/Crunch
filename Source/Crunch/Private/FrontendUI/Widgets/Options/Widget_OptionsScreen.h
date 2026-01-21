// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "FrontendUI/FrontendTypes/FrontendEnumTypes.h"
#include "FrontendUI/Widgets/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class UWidget_OptionsDetailsView;           // 右侧选项详细信息面板
class UListDataObject_Base;                 // 选项列表数据对象基类
class UFrontendCommonListView;              // 前端通用列表视图控件
class UFrontendTabListWidgetBase;           // 前端 Tab 列表控件（管理顶部选项卡）
class UOptionsDataRegistry;                 // 选项数据注册表（集中管理所有选项数据）

/**
 * 游戏选项设置主界面控件
 *
 * 这是玩家进入“设置/选项”菜单时显示的核心界面，负责：
 * - 顶部 Tab 切换（视频、音频、控制等）
 * - 中间选项列表展示与交互
 * - 右侧详细信息面板实时更新
 * - 处理重置、返回、保存等用户操作
 * - 协调数据注册与修改追踪
 *
 * 继承自 UWidget_ActivatableBase，支持通过 CommonUI 的激活/停用栈进行推送和弹出
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
protected:
	// ==================== UUserWidget 接口 ====================
	// Widget 在构造完成后调用，用于初始化绑定和事件注册
	virtual void NativeOnInitialized() override;
	// ===========================================================

	// ==================== UCommonActivatableWidget 接口 ====================
	// 当该界面被激活（推入前台显示）时调用
	virtual void NativeOnActivated() override;

	// 当该界面被停用（弹出或被其他界面覆盖）时调用
	virtual void NativeOnDeactivated() override;

	// 返回当前界面希望首先获得输入焦点的 Widget（用于手柄/键盘导航）
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	// ======================================================================
#pragma region 右下角的输入绑定
public:
	// ==================== 委托事件 ====================
	
	/** 
	 * 选项界面关闭时的委托
	 * 当用户点击返回按钮或触发返回动作时广播
	 * 可以在蓝图中绑定此事件来响应选项界面关闭
	 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOptionsScreenClosed);

	/** 选项界面关闭事件，可在蓝图中绑定 */
	UPROPERTY(BlueprintAssignable, Category = "Options Screen Events")
	FOnOptionsScreenClosed OnOptionsScreenClosed;
private:
	/** 
	 * 重置当前 Tab 下所有设置的输入动作配置
	 * 在项目设置的 Input Action Data Table 中定义，通常绑定为手柄的特定键
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen",
		meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	/** 重置按钮触发时的回调（弹出确认框并执行重置） */
	void OnResetBoundActionTriggered();

	/** 返回/退出按钮触发时的回调（关闭选项界面） */
	void OnBackBoundActionTriggered();
	
	// 保存“重置”输入动作的绑定句柄，便于动态添加/移除（控制是否显示重置提示）
	FUIActionBindingHandle ResetActionHandle;
#pragma endregion
#pragma region 选项界面数据处理
	/** 
	 * 当前选项界面拥有的数据注册表实例（单例模式）
	 * 使用 Transient 标记，表示不序列化，仅在运行时存在
	 * 通过 GetOrCreateDataRegistry() 延迟创建，避免不必要的开销
	 */
	UPROPERTY(Transient)
	UOptionsDataRegistry* CreatedOwningDataRegistry;
	
	// 获取或创建注册数据
	UOptionsDataRegistry* GetOrCreateDataRegistry();

	// 尝试获取选项列表项对应的详细信息面板类名
	FString TryGetEntryWidgetClassName(UObject* InOwningListItem) const;

	// ====================== 绑定的子控件 ======================
	/** 顶部选项 Tab 列表控件（视频、音频、游戏等分类标签） */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendTabListWidgetBase> TabListWidget_OptionsTabs;

	/** 中间主选项列表控件（显示当前 Tab 下的所有设置项） */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonListView> CommonListView_OptionsList;

	/** 右侧详细信息面板（显示选中或悬停选项的标题、描述、图片、禁用原因等） */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_OptionsDetailsView> DetailView_ListEntryInfo;
	// ===========================================================
	
	// 当玩家切换顶部 Tab 时调用，刷新列表内容并更新重置按钮状态
	UFUNCTION()
	void OnOptionsTabSelected(FName TabId);
	// 列表项悬停事件回调（更新右侧详情面板为悬停项信息）
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	// 列表项选中事件回调（更新右侧详情面板为选中项信息）
	void OnListViewItemSelected(UObject* InSelectedItem);

	/** 
	 * 收集当前 Tab 下所有已修改且支持重置的选项数据
	 * 用于决定是否显示“重置”按钮以及执行批量重置
	 */
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ResettableDataArray;

	// 当任意选项数据被修改时调用，用于动态更新 ResettableDataArray 和重置按钮显示
	void OnListViewListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifyReason);

	// 标记当前是否正在执行重置操作，用于避免递归或冲突修改
	bool bIsResettingData = false;
#pragma endregion
	
};
