// 幻雨喜欢小猫咪


#include "SessionEntryWidget.h"

void USessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SessionButton)
	{
		SessionButton->OnClicked.AddDynamic(this, &USessionEntryWidget::SessionEntrySelected);
	}
}

void USessionEntryWidget::InitializeEntry(const FString& Name, const FString& SessionIdStr, int32 CurrentPlayers, int32 MaxPlayers)
{
	// 缓存必要信息
	CachedSessionIdStr = SessionIdStr;
	CachedCurrentPlayers = CurrentPlayers;
	CachedMaxPlayers = MaxPlayers;

	// TODO：人数信息失败2025/09/03
	// 直接格式化并设置合并文本：房间名称 + 人数信息
	//FString CombinedText = FString::Printf(TEXT("%s (%d/%d)"), *Name, CurrentPlayers, MaxPlayers);
	// 展示移除掉人数信息，只保留房间名称
	FString CombinedText = Name;
	SessionNameText->SetText(FText::FromString(CombinedText));
	
	// 设置颜色
	UpdateRoomStatusColor();
}

void USessionEntryWidget::UpdateRoomStatusColor()
{
	if (!SessionNameText) return;
	
	// 根据房间状态设置不同的颜色
	if (CachedCurrentPlayers >= CachedMaxPlayers)
	{
		// 房间已满，显示高级红色（深红色，更有质感）
		SessionNameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.2f, 0.2f)));
	}
	else if (CachedCurrentPlayers >= CachedMaxPlayers * 0.8f)
	{
		// 房间接近满员，显示橙色
		SessionNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.5f, 0.0f)));
	}
	else
	{
		// 房间有空位，显示高级绿色（翠绿色，更柔和）
		SessionNameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.7f, 0.4f)));
	}
}

void USessionEntryWidget::SessionEntrySelected()
{
	OnSessionEntrySelected.Broadcast(CachedSessionIdStr);
}
