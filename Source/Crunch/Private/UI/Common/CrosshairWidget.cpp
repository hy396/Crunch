// 幻雨喜欢小猫咪


#include "CrosshairWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "GAS/Core/TGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 隐藏准星
	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		// 监听准星标签的变换
		OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Crosshair).AddUObject(this, &UCrosshairWidget::CrosshairTagUpdated);
		// 监听目标更新
		OwnerAbilitySystemComponent->GenericGameplayEventCallbacks.Add(TGameplayTags::Target_Updated).AddUObject(this, &UCrosshairWidget::TargetUpdated);
	}
	// 缓存玩家控制器
	CachedPlayerController = GetOwningPlayer();
	// 获取画布面板插槽用于定位
	CrosshairCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairCanvasPanelSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("十字准星控件必须放在Canvas Panel中才能正确定位"));
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	// 当准星可见时更新位置
	if (CrosshairImage->GetVisibility() == ESlateVisibility::Visible)
	{
		UpdateCrosshairPosition();
	}
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	// 根据标签计数显示/隐藏准星
	CrosshairImage->SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
	if (!CachedPlayerController || ! CrosshairCanvasPanelSlot) return;

	// 获取视口缩放比例
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);

	// 获取玩家控制器的视口大小
	int32 SizeX, SizeY;
	CachedPlayerController->GetViewportSize(SizeX, SizeY);

	// 如果没有目标居中设置
	if (!AimTarget)
	{
		FVector2D ViewportSize = FVector2D{static_cast<float>(SizeX), static_cast<float>(SizeY)};
		CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
		return;
	}

	// 有目标时追踪目标位置
	FVector2D TargetScreenPosition;
	CachedPlayerController->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(), TargetScreenPosition);
	if (TargetScreenPosition.X > 0 && TargetScreenPosition.X < SizeX && TargetScreenPosition.Y > 0 && TargetScreenPosition.Y < SizeY)
	{
		CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
	}
	
}

void UCrosshairWidget::TargetUpdated(const struct FGameplayEventData* EventData)
{
	// 更新当前目标
	AimTarget = EventData->Target;

	// 根据目标状态设置准星颜色
	CrosshairImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
}
