// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapActor.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

/**
 * 英雄联盟风格小地图Actor类
 * 实现类似英雄联盟的小地图功能
 */
UCLASS()
class CRUNCH_API AMinimapActor : public AActor
{
	GENERATED_BODY()

public:
	// 构造函数
	AMinimapActor();

	// 配置小地图参数
	void ConfigureMinimap(float InViewRange, float InViewHeight);

	// 获取渲染目标纹理
	UTextureRenderTarget2D* GetRenderTarget() const;

	// 更新小地图摄像机位置以跟随目标
	void UpdateMinimapPosition(const FVector& TargetLocation);

	// 获取小地图世界边界
	FVector2D GetMinimapWorldBounds() const;

	// 将世界坐标转换为小地图UV坐标
	FVector2D WorldToMinimapUV(const FVector& WorldLocation) const;

protected:
	// 游戏开始时调用
	virtual void BeginPlay() override;

private:
	// 场景捕获组件
	UPROPERTY(VisibleAnywhere, Category = "Minimap")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent;

	// 渲染目标纹理
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	// 小地图视野范围（半径）
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	float ViewRange = 2000.f;

	// 小地图摄像机高度
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	float ViewHeight = 1000.f;

	// 小地图世界边界（用于坐标转换）
	FVector2D MinimapWorldMin;
	FVector2D MinimapWorldMax;
};