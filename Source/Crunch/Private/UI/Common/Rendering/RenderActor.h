// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

/**
 * @class ARenderActor
 * 渲染专用Actor类，用于场景捕获和渲染到纹理
 * 
 * 主要功能：
 *  - 创建一个离屏渲染场景
 *  - 捕获指定目标的高质量图像
 *  - 将渲染结果输出到RenderTarget纹理
 */
UCLASS()
class CRUNCH_API ARenderActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARenderActor();
	/**
	 * 设置渲染目标纹理
	 * @param RenderTarget 要渲染到的目标纹理
	 */
	void SetRenderTarget(UTextureRenderTarget2D* RenderTarget);
	
	/**
	 * 执行场景捕获操作
	 * 调用后会将当前场景渲染到预设的RenderTarget
	 */
	void UpdateRender();
	
	// 获取场景捕获组件（用于进一步配置）
	FORCEINLINE USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComponent; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/** 根组件，用于组织场景层次 */
	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	TObjectPtr<USceneComponent> RootComp;

	/**
	 * 场景捕获组件，核心渲染功能
	 * 将3D场景渲染为2D纹理
	 */
	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;
};
