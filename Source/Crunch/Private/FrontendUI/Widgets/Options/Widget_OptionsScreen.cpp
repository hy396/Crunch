// 幻雨喜欢小猫咪


#include "Widget_OptionsScreen.h"

#include "HuanYuDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "OptionsDataRegistry.h"
#include "Widget_OptionsDetailsView.h"
#include "DataObjects/ListDataObject_Collection.h"
#include "FrontendUI/FrontendSettings/FrontendGameUserSettings.h"
#include "FrontendUI/Subsystems/FrontendUISubsystem.h"
#include "FrontendUI/Widgets/Components/FrontendCommonButtonBase.h"
#include "FrontendUI/Widgets/Components/FrontendCommonListView.h"
#include "FrontendUI/Widgets/Components/FrontendTabListWidgetBase.h"
#include "ListEntries/Widget_ListEntry_Base.h"

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

	// 选项标签切换事件绑定
	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
	
	// 悬停事件绑定
	CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::OnListViewItemHovered);
	// 选中事件绑定
	CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnListViewItemSelected);
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
	// 保存用户在选项界面中修改的所有设置参数
	// 参数 true 表示立即应用并保存到本地配置文件
	UFrontendGameUserSettings::Get()->ApplySettings(true);
}

UWidget* UWidget_OptionsScreen::NativeGetDesiredFocusTarget() const
{
	// 获取选项列表中当前选中的数据项（如果是 UObject* 类型）
	if (UObject* SelectedObject = CommonListView_OptionsList->GetSelectedItem())
	{
		// 根据选中的数据项获取对应的 Entry Widget（列表中的每一行 Widget）
		if (UUserWidget* SelectedEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(SelectedObject))
		{
			// 如果找到了对应的 Entry Widget，则将其作为焦点目标返回
			// 这样离开后再回到选项界面时，会自动聚焦到上次选中的选项行
			return SelectedEntryWidget;
		}
	}

	// 如果没有选中项或无法获取对应的 Widget，则回退到父类的默认焦点逻辑
	return Super::NativeGetDesiredFocusTarget();
}

/**
 * 触发“重置当前 Tab 下所有设置”操作
 *
 * 会先弹出确认窗口，只有用户确认后才执行实际重置逻辑。
 */
void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	// 检查是否有可重置的数据，如果没有则直接返回
	if (ResettableDataArray.IsEmpty()) return;

	// 获取当前激活的标签页按钮
	UCommonButtonBase* SelectedTabButton = TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabListWidget_OptionsTabs->GetActiveTab());
	// 获取当前标签页的显示文本，并转换为字符串
	const FString SelectedTabButtonName = CastChecked<UFrontendCommonButtonBase>(SelectedTabButton)->GetButtonDisplayText().ToString();
	// 调用UI子系统，推送确认屏幕到模态堆栈
	// 参数1: 确认屏幕类型为Yes/No（是/否）
	// 参数2: 确认屏幕标题为"Reset"
	// 参数3: 确认屏幕消息为询问用户是否确认重置当前标签页设置
	// 参数4: 按钮点击回调函数，当用户点击确认或取消时执行
	UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		EConfirmScreenType::YesNo,
		FText::FromString(TEXT("重置")),	// Reset
		FText::FromString(TEXT("你确定要重置 ")+ SelectedTabButtonName + TEXT(" 标签页下的所有设置吗？")),//		// FText::FromString(TEXT("Are you sure you want to reset all the setting under the ")+ SelectedTabButtonName + TEXT(" tab.")),
	[this](EConfirmScreenButtonType ClickedButtonType)  // Lambda表达式，捕获当前对象(this)，无参数，无返回值
		{
			// 检查用户是否点击了确认按钮，如果不是确认按钮则直接返回，不执行重置操作
			if (ClickedButtonType != EConfirmScreenButtonType::Confirmed) return; // 意味着没有点击确认按钮

			// ----- 下面的操作是点击了确认按钮的操作 -----
			// 设置正在重置数据的标志，防止在此期间进行其他操作
			bIsResettingData = true;
			// 初始化一个布尔变量，用于跟踪是否有数据重置失败
			bool bHasDataFailedToReset = false;
			// 遍历所有可重置的数据项
			for (UListDataObject_Base* DataToReset : ResettableDataArray)
			{
				// 检查数据对象是否有效，无效则跳过
				if (!DataToReset) continue;

				// 尝试将数据重置为默认值
				if (DataToReset->TryResetBackToDefaultValue())
				{
					// 重置成功，打印成功信息到调试输出
					Debug::Print(DataToReset->GetDataDisplayName().ToString() + TEXT(" 已重置"));
				}else
				{
					// 重置失败，设置失败标志，并打印失败信息到调试输出
					bHasDataFailedToReset = true;
					Debug::Print(DataToReset->GetDataDisplayName().ToString() + TEXT(" 重置失败"));
				}
			}
			// 检查是否所有数据都成功重置
			if (!bHasDataFailedToReset)
			{
				// 如果没有数据重置失败，清空可重置数据数组
				ResettableDataArray.Empty();
				// 移除重置动作绑定，隐藏重置按钮
				RemoveActionBinding(ResetActionHandle);
			}
			// 重置操作完成，将重置标志设为false
			bIsResettingData = false;
		}
		);
	// Debug::Print(TEXT("重置绑定操作已触发"));
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

FString UWidget_OptionsScreen::TryGetEntryWidgetClassName(UObject* InOwningListItem) const
{
	if (UUserWidget* FoundEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(InOwningListItem))
	{
		return FoundEntryWidget->GetClass()->GetName(); // 返回 Widget 类名
	}
	return TEXT("条目控件无效"); // 未找到有效 Widget 时返回提示字符串
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabId)
{
	// 清空右侧详细信息面板的内容
	DetailView_ListEntryInfo->ClearDetailsViewInfo();
//	Debug::Print(TEXT("OnOptionsTabSelected--ID:") + TabId.ToString());

	// 根据选中的 Tab ID 获取对应的选项数据列表
	TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabId);

	// 将数据设置到选项列表控件中
	CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	CommonListView_OptionsList->RequestRefresh(); // 刷新显示

	if (CommonListView_OptionsList->GetNumItems() != 0)  // 如果有数据
	{
		CommonListView_OptionsList->NavigateToIndex(0);  // 导航第一个
		CommonListView_OptionsList->SetSelectedIndex(0);  // 将第一个设为选择状态
	}

	// 清空可重置数据数组，重新收集当前 Tab 下支持重置的项
	ResettableDataArray.Empty();
	for (UListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	{
		if (!FoundListSourceItem) continue;

		// 绑定数据修改事件（避免重复绑定）
		if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModified.AddUObject(this, &ThisClass::OnListViewListDataModified);
		}

		// 如果该数据项支持重置到默认值，则加入可重置列表
		if (FoundListSourceItem->CanResetBackToDefaultValue())
		{
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}

	// 根据可重置项的数量决定是否显示“重置”按钮
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);  // 如果重置的列表是空的则不显示重置按钮
	}else
	{
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);  // 如果有可以重置的且没有重置按钮 则添加重置按钮
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem) return;

	// const FString DebugString = CastChecked<UListDataObject_Base>(InHoveredItem)->GetDataDisplayName().ToString() +
	// 	TEXT("是 ：") + (bWasHovered? TEXT("悬停状态") : TEXT("非悬停状态"));
	//
	// Debug::Print(DebugString);
	// 获取悬停项对应的 Entry Widget
	UWidget_ListEntry_Base* HoveredEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem<UWidget_ListEntry_Base>(InHoveredItem);
	check(HoveredEntryWidget);

	// 通知 Entry Widget 自身处理悬停状态变化
	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

	// 悬停时：更新右侧详细信息面板显示当前悬停项的信息
	if (bWasHovered)
	{
		DetailView_ListEntryInfo->UpdateDetailsViewInfo(
			CastChecked<UListDataObject_Base>(InHoveredItem),
			TryGetEntryWidgetClassName(InHoveredItem));
	}
	else // 取消悬停时：恢复显示当前选中项的信息
	{
		if (UListDataObject_Base* SelectedItem = CommonListView_OptionsList->GetSelectedItem<UListDataObject_Base>())
		{
			DetailView_ListEntryInfo->UpdateDetailsViewInfo(
				SelectedItem,
				TryGetEntryWidgetClassName(SelectedItem));
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem) return;
	const FString DebugString = CastChecked<UListDataObject_Base>(InSelectedItem)->GetDataDisplayName().ToString() +TEXT("是选中状态");
	
	Debug::Print(DebugString);
	// 更新详细信息面板为当前选中项的内容
	DetailView_ListEntryInfo->UpdateDetailsViewInfo(
		CastChecked<UListDataObject_Base>(InSelectedItem),
			TryGetEntryWidgetClassName(InSelectedItem)
			);
}

void UWidget_OptionsScreen::OnListViewListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	// 无效数据或正在重置过程中，忽略本次修改
	if (!ModifiedData || bIsResettingData) return;

	// 如果修改后的数据现在支持重置，则加入可重置列表并显示重置按钮
	if (ModifiedData->CanResetBackToDefaultValue())
	{
		ResettableDataArray.AddUnique(ModifiedData);
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
	else // 如果不再支持重置，则从列表中移除
	{
		if (ResettableDataArray.Contains(ModifiedData))
		{
			ResettableDataArray.Remove(ModifiedData);
		}
	}

	// 如果最终无可重置项，隐藏重置按钮
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
}
