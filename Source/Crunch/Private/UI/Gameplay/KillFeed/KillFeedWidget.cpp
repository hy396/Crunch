// 幻雨喜欢小猫咪

#include "KillFeedWidget.h"
#include "Player/MPlayerState.h"
#include "KillFeedItemWidget.h"
#include "Components/VerticalBoxSlot.h"

void UKillFeedWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UKillFeedWidget::AddKillFeed(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates)
{
	if (!KillFeedItemWidgetClass || !FeedContainer)
	{
		return;
	}

	// 创建新的击杀通报条目
	UKillFeedItemWidget* NewFeedItem = CreateWidget<UKillFeedItemWidget>(this, KillFeedItemWidgetClass);
	if (NewFeedItem)
	{
		// 设置数据
		NewFeedItem->SetKillFeedData(KillerState, VictimState, AssistStates);

		// 添加到容器顶部（最新的在最上面）
		// 设置水平对齐方式为右对齐
		if (UVerticalBoxSlot* FeedSlot = FeedContainer->AddChildToVerticalBox(NewFeedItem))
		{
			FeedSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
		}
	}

	// 移除超出最大数量的旧条目
	while (FeedContainer->GetChildrenCount() > MaxFeedItems)
	{
		if (UWidget* FirstChild = FeedContainer->GetChildAt(0))
		{
			FirstChild->RemoveFromParent();
		}
	}
}

void UKillFeedWidget::ClearAllFeed()
{
	if (FeedContainer)
	{
		FeedContainer->ClearChildren();
	}
}