// 幻雨喜欢小猫咪


#include "TeamSelectionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTeamSelectionWidget::SetSlotID(uint8 NewSlotID)
{
	SlotID = NewSlotID;
}

void UTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName)
{
	// 显示玩家昵称
	InfoText->SetText(FText::FromString(PlayerNickName));
}

void UTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 将按钮控件的"OnClicked"事件动态绑定到本类的SelectButtonClicked函数。
	// 当用户点击按钮时，就会调用SelectButtonClicked。
	SelectButton->OnClicked.AddDynamic(this, &UTeamSelectionWidget::SelectButtonClicked);
}

void UTeamSelectionWidget::SelectButtonClicked()
{
	// 广播委托
	OnSlotClicked.Broadcast(SlotID);
}
