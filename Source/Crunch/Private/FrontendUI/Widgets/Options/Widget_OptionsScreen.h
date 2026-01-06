// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "FrontendUI/Widgets/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class UFrontendTabListWidgetBase;
class UOptionsDataRegistry;
/**
 * Options 设置界面主小部件
 *
 * 负责：
 * - 管理选项 Tab
 * - 展示选项列表
 * - 响应用户输入
 * - 协调重置 / 保存等操作
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
protected:
	// ~Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~End UUserWidget Interface

	// ~Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	// ~End UCommonActivatableWidget Interface
private:
#pragma region 右下角的输入绑定
	/** 重置设置输入动作 */
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen",
		meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	/** 重置 / 返回操作回调 */
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();

	FUIActionBindingHandle ResetActionHandle;
#pragma endregion
#pragma region 选项界面数据处理
	// 处理在选项界面创建数据的过程，通过这个变量,不能直接与这个变量进行交互
	UPROPERTY(Transient)
	UOptionsDataRegistry* CreatedOwningDataRegistry;
	
	//获取或创建注册数据
	UOptionsDataRegistry* GetOrCreateDataRegistry();

	// *** 绑定小部件 *** //
	// 选项界面的选项Tab
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendTabListWidgetBase> TabListWidget_OptionsTabs;
	// UPROPERTY(meta = (BindWidget))
	// TObjectPtr<UFrontendCommonListView> CommonListView_OptionsList;
	// UPROPERTY(meta = (BindWidget))
	// TObjectPtr<UWidget_OptionsDetailsView> DetailView_ListEntryInfo;
	// *** 绑定小部件 *** //
	
	// tab标签选择时显示内容
	UFUNCTION()
	void OnOptionsTabSelected(FName TabId);
	// 悬停事件回调
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	// 选中事件回调
	void OnListViewItemSelected(UObject* InSelectedItem);
#pragma endregion
	
};
