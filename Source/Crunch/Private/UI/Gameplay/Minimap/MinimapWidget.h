// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class AMinimapActor;
class AActor;
class UImage;
class UCanvasPanel;
class UTexture2D;

/**
 * 小地图UI控件
 * 负责显示小地图和单位标记
 */
UCLASS()
class CRUNCH_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 原生构造
	virtual void NativeConstruct() override;
	// 原生析构
	virtual void NativeDestruct() override;

	// 配置小地图参数
	UFUNCTION(BlueprintCallable)
	void ConfigureMinimap(float ViewRange, float ViewHeight);

	// 设置要跟随的目标
	UFUNCTION(BlueprintCallable)
	void SetTargetToFollow(AActor* Target);

	// 添加单位标记
	void AddUnitMarker(AActor* Unit, UTexture2D* Icon, FLinearColor Color);

	// 移除单位标记
	void RemoveUnitMarker(AActor* Unit);

	// 更新单位标记位置
	void UpdateUnitMarkerPosition(AActor* Unit);

	// 检查并更新单位状态（死亡/复活）
	void CheckAndUpdateUnitStatus(AActor* Unit);

protected:
	// 原生鼠标按下事件
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	// 生成小地图Actor
	void SpawnMinimapActor();

	// 小地图更新定时器回调
	void OnMinimapUpdateTimer();

	// 更新小地图位置
	void UpdateMinimapPosition();

	// 处理小地图点击
	void HandleMinimapClick(const FPointerEvent& InMouseEvent);

	// 检查单位是否死亡
	bool IsUnitDead(AActor* Unit) const;

private:
	// 小地图Actor类
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<AMinimapActor> MinimapActorClass;

	// 小地图Actor实例
	UPROPERTY()
	TObjectPtr<AMinimapActor> MinimapActor;

	// 小地图图像控件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MinimapImage;

	// 单位标记容器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> UnitMarkersContainer;

	// 渲染目标参数名称
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FName RenderTargetParamName = "RenderTarget";

	// 小地图视图范围
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	float MinimapViewRange = 2000.0f;

	// 小地图视图高度
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	float MinimapViewHeight = 1000.0f;

	// 单位标记大小
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FVector2D UnitMarkerSize = FVector2D(20.0f, 20.0f);

	// 死亡标签名称
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	FName DeathTagName = "State.Dead";

	// 要跟随的目标
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetToFollow;

	// 单位标记映射（单位 -> 图像控件）
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, TObjectPtr<UImage>> UnitMarkers;

	// 单位死亡状态映射（单位 -> 是否死亡）
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, bool> UnitDeathStatus;

	// 小地图更新定时器句柄
	FTimerHandle MinimapUpdateTimerHandle;
};