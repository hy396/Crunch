// 幻雨喜欢小猫咪


#include "Widget_OptionsScreen.h"

#include "HuanYuDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "OptionsDataRegistry.h"
#include "DataObjects/ListDataObject_Collection.h"
#include "FrontendUI/Widgets/Components/FrontendTabListWidgetBase.h"

void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 重置操作
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered)
			)
		);
	}
	// 注册后默认的回退返回按钮操作  有个这个不需要勾选WBP_CAW_OptionsScreen中is back handle 和is back action displayer in action bar
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this, &ThisClass::OnBackBoundActionTriggered)
		)
	);

	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	// 遍历数据注册表中所有已注册的选项标签集合（Tab Collections）
	for (UListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		// 安全检查：如果集合对象为空，则跳过本次循环
		if (!TabCollection)
		{
			continue;
		}

		// 获取当前 Tab 集合的唯一 ID（FName 类型）
		const FName TabID = TabCollection->GetDataID();
		// 获取当前 Tab 集合的显示名称（用于 UI 上显示的文字）
		const FText TabDis = TabCollection->GetDataDisplayName();

		// 检查选项标签列表控件中是否已经存在对应 ID 的 Tab 按钮
		// 如果已经存在，则无需重复注册，直接跳过
		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}
		
		// 向选项标签列表控件请求注册一个新的 Tab
		// 传入 Tab 的 ID 和显示名称，控件会据此创建对应的按钮
		TabListWidget_OptionsTabs->RequestRegisterTab(TabID, TabDis);
	}
}

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

UWidget* UWidget_OptionsScreen::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

/**
 * 触发“重置当前 Tab 下所有设置”操作
 *
 * 会先弹出确认窗口，只有用户确认后才执行实际重置逻辑。
 */
void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("重置绑定操作已触发"));
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	// 返回操作：关闭 Options 界面
	DeactivateWidget();
	// Debug::Print(TEXT("back bound action triggered"));
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!CreatedOwningDataRegistry) // 变量无效将成创建
	{
		CreatedOwningDataRegistry = NewObject<UOptionsDataRegistry>();
		// 初始化数据注册
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}
	checkf(CreatedOwningDataRegistry, TEXT("配置选项屏幕的数据注册无效"));

	return CreatedOwningDataRegistry;
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	Debug::Print(TEXT("OnOptionsTabSelected--ID:") + TabId.ToString());
}

void UWidget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
}
