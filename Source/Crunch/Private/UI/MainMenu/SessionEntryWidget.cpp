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

void USessionEntryWidget::InitializeEntry(const FString& Name, const FString& SessionIdStr)
{
	// 设置会话名称
	SessionNameText->SetText(FText::FromString(Name));
	// 缓存 SessionId
	CachedSessionIdStr = SessionIdStr;
}

void USessionEntryWidget::SessionEntrySelected()
{
	OnSessionEntrySelected.Broadcast(CachedSessionIdStr);
}
