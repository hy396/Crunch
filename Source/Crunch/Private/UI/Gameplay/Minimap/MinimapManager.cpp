// 幻雨喜欢小猫咪

#include "MinimapManager.h"
#include "MinimapWidget.h"

void UMinimapManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UMinimapManager::RegisterMinimapWidget(UMinimapWidget* Widget)
{
	if (!Widget) return;

	// 检查是否已注册
	for (const TWeakObjectPtr<UMinimapWidget>& RegisteredWidget : RegisteredMinimaps)
	{
		if (RegisteredWidget.IsValid() && RegisteredWidget.Get() == Widget)
		{
			return; // 已注册
		}
	}

	// 添加到注册列表
	RegisteredMinimaps.Add(Widget);
}

void UMinimapManager::UnregisterMinimapWidget(UMinimapWidget* Widget)
{
	if (!Widget) return;

	// 从注册列表中移除
	for (int32 i = RegisteredMinimaps.Num() - 1; i >= 0; --i)
	{
		if (RegisteredMinimaps[i].IsValid() && RegisteredMinimaps[i].Get() == Widget)
		{
			RegisteredMinimaps.RemoveAt(i);
			break;
		}
	}
}

void UMinimapManager::AddUnitToMinimaps(AActor* Unit, UTexture2D* Icon, const FLinearColor& Color)
{
	if (!Unit) return;

	// 添加单位到所有注册的小地图
	for (const TWeakObjectPtr<UMinimapWidget>& MinimapWidget : RegisteredMinimaps)
	{
		if (MinimapWidget.IsValid())
		{
			MinimapWidget->AddUnitMarker(Unit, Icon, Color);
		}
	}
}

void UMinimapManager::RemoveUnitFromMinimaps(AActor* Unit)
{
	if (!Unit) return;

	// 从所有注册的小地图中移除单位
	for (const TWeakObjectPtr<UMinimapWidget>& MinimapWidget : RegisteredMinimaps)
	{
		if (MinimapWidget.IsValid())
		{
			MinimapWidget->RemoveUnitMarker(Unit);
		}
	}
}

void UMinimapManager::UpdateUnitStatusOnMinimaps(AActor* Unit)
{
	if (!Unit) return;

	// 更新所有注册的小地图上的单位状态
	for (const TWeakObjectPtr<UMinimapWidget>& MinimapWidget : RegisteredMinimaps)
	{
		if (MinimapWidget.IsValid())
		{
			MinimapWidget->CheckAndUpdateUnitStatus(Unit);
		}
	}
}