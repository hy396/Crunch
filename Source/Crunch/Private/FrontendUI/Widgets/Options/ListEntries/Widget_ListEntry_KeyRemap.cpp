// 幻雨喜欢小猫咪


#include "Widget_ListEntry_KeyRemap.h"

#include "FrontendUI/FrontendFunctionLibrary.h"
#include "FrontendUI/Core/FrontendGameplayTags.h"
#include "FrontendUI/Subsystems/FrontendUISubsystem.h"
#include "FrontendUI/Widgets/Components/FrontendCommonButtonBase.h"
#include "FrontendUI/Widgets/Options/DataObjects/ListDataObject_KeyRemap.h"

/**
 * 当拥有的列表数据对象设置时调用，用于初始化按键重映射控件
 * @param InOwningListDataObject 拥有的列表数据对象
 */
void UWidget_ListEntry_KeyRemap::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	CachedOwningKeyRemapDataObject = CastChecked<UListDataObject_KeyRemap>(InOwningListDataObject);
	CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
}

/**
 * 当拥有的列表数据对象修改时调用，更新按键显示
 * @param OwningModifiedData 拥有的修改后的数据对象
 * @param ModifyReason 修改原因
 */
void UWidget_ListEntry_KeyRemap::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	Super::OnOwningListDataObjectModified(OwningModifiedData, ModifyReason);
	if (CachedOwningKeyRemapDataObject){
		CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
		}
}

/**
 * 初始化组件并绑定按钮点击事件
 */
void UWidget_ListEntry_KeyRemap::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CommonButton_RemapKey->OnClicked().AddUObject(this, &ThisClass::OnRemapKeyButtonClicked);
	CommonButton_ResetKeyBinding->OnClicked().AddUObject(this, &ThisClass::OnResetKeyBindingButtonClicked);
	
}

/**
 * 处理重映射按键按钮点击事件
 */
void UWidget_ListEntry_KeyRemap::OnRemapKeyButtonClicked()
{
	SelectThisEntryWidget(); // 为了高亮点击的该行
	// Debug::Print(TEXT("点击了映射按键"));
	UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		FrontendGameplayTags::WidgetStack::Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Widget::KeyRemapScreen),
		[this](EAsyncPushWidgetState PushState, UWidget_ActivatableBase* PushedWidget)
		{
			if (PushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				// UWidget_KeyRemapScreen* CreatedKeyRemapScreen = CastChecked<UWidget_KeyRemapScreen>(PushedWidget);
				// CreatedKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindUObject(this, &ThisClass::OnKeyRemapPressed);
				// CreatedKeyRemapScreen->OnKeyRemapScreenKeySelectCanceled.BindUObject(this, &ThisClass::OnKeyRemapCanceled);
				// if (CachedOwningKeyRemapDataObject)
				// {
				// 	CreatedKeyRemapScreen->SetDesiredInputTypeToFilter(CachedOwningKeyRemapDataObject->GetDesiredInputKeyType());
				// }
				
			}
		}
	);
}

/**
 * 处理重置按键绑定按钮点击事件
 */
void UWidget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked()
{
	if (!CachedOwningKeyRemapDataObject) return;
	// Debug::Print(TEXT("点击了映射重置按键"));
	SelectThisEntryWidget(); // 为了高亮点击的该行
	// 如果没有改动告诉玩家已经是默认键无需改动
	if (!CachedOwningKeyRemapDataObject->CanResetBackToDefaultValue())
	{
		UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(EConfirmScreenType::Ok,
			FText::FromString(TEXT("重置映射按钮")),
			FText::FromString(CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT("已经是默认值，无需设置")),
			[](EConfirmScreenButtonType ClickedButton){});
		return;
	}
	
	// 重置为默认键
	UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(EConfirmScreenType::YesNo,FText::FromString(TEXT("重置映射按钮")),
	FText::FromString(TEXT("您确定要重置") + CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT("按键吗？")),
	[this](EConfirmScreenButtonType ClickedButton)
	{
		if (ClickedButton == EConfirmScreenButtonType::Confirmed)
		{
			CachedOwningKeyRemapDataObject->TryResetBackToDefaultValue();
		}
	});
}

/**
 * 处理按键重映射事件
 * @param PressedKey 按下的键
 */
void UWidget_ListEntry_KeyRemap::OnKeyRemapPressed(const FKey& PressedKey)
{
	// Debug::Print(TEXT("检测到映射键： ") + PressedKey.GetDisplayName().ToString());
	if (CachedOwningKeyRemapDataObject)
	{
		CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
	}
}

/**
 * 处理按键重映射取消事件
 * @param CanceledReason 取消原因
 */
void UWidget_ListEntry_KeyRemap::OnKeyRemapCanceled(const FString& CanceledReason)
{
	UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		EConfirmScreenType::Ok,
		FText::FromString(TEXT("Key Remap")),
		FText::FromString(CanceledReason),
		[](EConfirmScreenButtonType ClickedButton)
		{
			
		}
		);
}