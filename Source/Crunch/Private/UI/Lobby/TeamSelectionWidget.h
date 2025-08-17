// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UTextBlock;
class UButton;
// 声明一个委托：当这个插槽被点击时广播出去，并携带这个插槽的ID。
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /*SlotID*/);

/**
 * 用于表示队伍选择界面中一个可选槽位（Slot）的UI控件。
 */
UCLASS()
class CRUNCH_API UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 设置此UI控件代表的槽位ID。
	void SetSlotID(uint8 NewSlotID);

	// 更新此槽位显示的信息（通常是玩家昵称）。
	void UpdateSlotInfo(const FString& PlayerNickName);

	// 重写原生构建函数，用于初始化绑定。
	virtual void NativeConstruct() override;

	// 当用户点击这个槽位的选择按钮时，会广播此委托。
	FOnSlotClicked OnSlotClicked;

private:
	// 用于选择此槽位的按钮。
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SelectButton;  

	// 用于显示槽位信息（如玩家名）的文本控件。
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> InfoText;  

	// UFUNCTION宏标记，用于绑定按钮点击事件。
	UFUNCTION()
	void SelectButtonClicked(); // 内部处理按钮点击的函数。
	
	// 此控件所代表的槽位的唯一标识符（ID）。
	uint8 SlotID;  
};
