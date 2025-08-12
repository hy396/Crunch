// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "CrosshairWidget.generated.h"

class UImage;
class UCanvasPanelSlot;
/**
 * 准星控件
 * 用于显示和动态更新准星状态（如是否有目标、准星颜色等）
 */
UCLASS()
class CRUNCH_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构建时回调，初始化控件
	virtual void NativeConstruct() override;

	// 每帧回调，处理准星位置和状态更新
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// 有目标时准星颜色
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor HasTargetColor = FLinearColor::Red;

	// 无目标时准星颜色
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor NoTargetColor = FLinearColor::White;

	// 准星图片控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> CrosshairImage;

	// 准星标签变化回调（用于响应目标锁定等）
	void CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount);

	// 准星在画布上的Slot（用于定位）
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CrosshairCanvasPanelSlot;

	// 缓存的玩家控制器
	UPROPERTY()
	TObjectPtr<APlayerController> CachedPlayerController;

	// 更新准星位置
	void UpdateCrosshairPosition();

	// 当前瞄准目标
	UPROPERTY()
	TObjectPtr<const AActor> AimTarget;

	// 目标更新回调（用于响应目标变化事件）
	void TargetUpdated(const struct FGameplayEventData* EventData);
};
