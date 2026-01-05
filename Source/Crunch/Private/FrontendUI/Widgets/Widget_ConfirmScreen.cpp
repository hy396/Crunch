// 幻雨喜欢小猫咪


#include "Widget_ConfirmScreen.h"

#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Components/FrontendCommonButtonBase.h"

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOKScreen(const FText& InScreenTitle,
                                                                   const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo OKButtonInfo;
	OKButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	OKButtonInfo.ButtonTextToDisplay = FText::FromString("OK");

	InfoObject->AvailableScreenButtons.Add(OKButtonInfo);

	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateYesNoScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo YesButtonInfo;
	YesButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	YesButtonInfo.ButtonTextToDisplay = FText::FromString("Yes");
	InfoObject->AvailableScreenButtons.Add(YesButtonInfo);

	FConfirmScreenButtonInfo NoButtonInfo;
	NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	NoButtonInfo.ButtonTextToDisplay = FText::FromString("No");
	InfoObject->AvailableScreenButtons.Add(NoButtonInfo);
	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOKCancelScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
	InfoObject->ScreenTitle = InScreenTitle;
	InfoObject->ScreenMessage = InScreenMessage;

	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	OkButtonInfo.ButtonTextToDisplay = FText::FromString("Ok");
	InfoObject->AvailableScreenButtons.Add(OkButtonInfo);

	FConfirmScreenButtonInfo CancelButtonInfo;
	CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	CancelButtonInfo.ButtonTextToDisplay = FText::FromString("Cancel");
	InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);
	return InfoObject;
}

void UWidget_ConfirmScreen::InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject,
	TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback)
{
	// 确保传入的参数和控件有效
	check(InScreenInfoObject && CommonTextBlock_Title && CommonTextBlock_Message && DynamicEntryBox_Buttons);

	// 设置标题和消息文本
	CommonTextBlock_Title->SetText(InScreenInfoObject->ScreenTitle);
	CommonTextBlock_Message->SetText(InScreenInfoObject->ScreenMessage);

	// 检查动态按钮框是否已包含之前创建的按钮
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		// 清除所有按钮和输入框的标题，并且为输入框制定了小部件类型 在创建子小部件蓝图时
		DynamicEntryBox_Buttons->Reset<UFrontendCommonButtonBase>(
			[](UFrontendCommonButtonBase& ExistingButton)
			{
				ExistingButton.OnClicked().Clear();
			}
		);
	}
	// 确保可用按钮列表不为空
	check(!InScreenInfoObject->AvailableScreenButtons.IsEmpty());
	
	// 遍历并创建所有可用按钮
	for (const FConfirmScreenButtonInfo& AvailableButtonInfo : InScreenInfoObject->AvailableScreenButtons)
	{
		// FDataTableRowHandle InputActionRowHandle;
		// switch (AvailableButtonInfo.ConfirmScreenButtonType)
		// {
		// 	// 注销下面三行是因为用手柄时，当在确认界面时，焦点在"否"上，点击手柄上的确认键会直接结束游戏，应该关掉确认界面，所以注销掉下面三行， 确认键CommonUI会自动处理确认键
		// 	// case EConfirmScreenButtonType::Confirmed:
		// 	// 	InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultClickAction(); // 获取默认点击操作
		// 	// 	break;
		// 	case EConfirmScreenButtonType::Cancelled:
		// 		InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();  // 获取默认返回操作
		// 		break;
		// 	case EConfirmScreenButtonType::Closed:
		// 		InputActionRowHandle = ICommonInputModule::GetSettings().GetDefaultBackAction();   // 获取默认返回操作
		// 		break;
		// default:
		// 	break;
		// 	break;
		// }
		
		// 创建按钮并设置其文本
		UFrontendCommonButtonBase* AddedButton = DynamicEntryBox_Buttons->CreateEntry<UFrontendCommonButtonBase>();
		AddedButton->SetButtonText(AvailableButtonInfo.ButtonTextToDisplay);
		// AddedButton->SetTriggeredInputAction(InputActionRowHandle);

		// 为按钮添加点击事件处理
		AddedButton->OnClicked().AddLambda(
		[ClickedButtonCallback, AvailableButtonInfo, this]()
		{
			// 执行回调函数并将按钮类型传递给它
			ClickedButtonCallback(AvailableButtonInfo.ConfirmScreenButtonType);
			// 停用当前小部件（关闭确认屏幕）
			DeactivateWidget();
		});
	}
}

UWidget* UWidget_ConfirmScreen::NativeGetDesiredFocusTarget() const
{
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		// 如果有两个按钮 聚焦在最后一个按钮上 最后一个按钮是 否
		DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();
	}
	return Super::NativeGetDesiredFocusTarget();
}
