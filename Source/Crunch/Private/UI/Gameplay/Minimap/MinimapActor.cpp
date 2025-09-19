// 幻雨喜欢小猫咪

#include "MinimapActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

// 构造函数
AMinimapActor::AMinimapActor()
{
	// 设置此Actor不每帧调用Tick()
	PrimaryActorTick.bCanEverTick = false;

	// 创建根组件
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// 创建场景捕获组件
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(RootComponent);

	// 配置场景捕获组件
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
	SceneCaptureComponent->bCaptureEveryFrame = true;
	SceneCaptureComponent->bCaptureOnMovement = false;
	SceneCaptureComponent->LODDistanceFactor = 0.1f; // 降低LOD以提高性能
}

void AMinimapActor::ConfigureMinimap(float InViewRange, float InViewHeight)
{
	ViewRange = InViewRange;
	ViewHeight = InViewHeight;

	// 设置正交宽度
	SceneCaptureComponent->OrthoWidth = ViewRange * 2.f;

	// 计算世界边界
	MinimapWorldMin = FVector2D(-ViewRange, -ViewRange);
	MinimapWorldMax = FVector2D(ViewRange, ViewRange);

	// 更新渲染目标大小
	if (RenderTarget)
	{
		RenderTarget->ResizeTarget(ViewRange * 2.f, ViewRange * 2.f);
	}
}

void AMinimapActor::BeginPlay()
{
	Super::BeginPlay();

	// 创建渲染目标纹理
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(ViewRange * 2.f, ViewRange * 2.f);
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB;
	RenderTarget->ClearColor = FLinearColor(0.1f, 0.1f, 0.1f, 1.0f); // 深色背景
	RenderTarget->UpdateResource();

	// 将渲染目标分配给场景捕获组件
	SceneCaptureComponent->TextureTarget = RenderTarget;

	// 设置摄像机旋转为俯视视角
	SceneCaptureComponent->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));

	// 配置可见性设置
	SceneCaptureComponent->ShowFlagSettings.Add(FEngineShowFlagsSetting(TEXT("PostProcessing"), false));
	SceneCaptureComponent->ShowFlagSettings.Add(FEngineShowFlagsSetting(TEXT("AntiAliasing"), false));
	SceneCaptureComponent->ShowFlagSettings.Add(FEngineShowFlagsSetting(TEXT("MotionBlur"), false));
}

UTextureRenderTarget2D* AMinimapActor::GetRenderTarget() const
{
	return RenderTarget;
}

void AMinimapActor::UpdateMinimapPosition(const FVector& TargetLocation)
{
	// 设置小地图摄像机位置
	FVector MinimapPosition = TargetLocation;
	MinimapPosition.Z += ViewHeight; // 将摄像机放置在目标上方
	SceneCaptureComponent->SetWorldLocation(MinimapPosition);
}

FVector2D AMinimapActor::GetMinimapWorldBounds() const
{
	return FVector2D(ViewRange, ViewRange);
}

FVector2D AMinimapActor::WorldToMinimapUV(const FVector& WorldLocation) const
{
	// 获取小地图摄像机位置
	FVector CameraLocation = SceneCaptureComponent->GetComponentLocation();
	
	// 计算相对于摄像机的位置
	FVector RelativeLocation = WorldLocation - CameraLocation;
	
	// 转换为小地图坐标系（忽略Z轴）
	FVector2D MinimapRelative(RelativeLocation.X, RelativeLocation.Y);
	
	// 转换为UV坐标 (0-1范围)
	FVector2D UVCoord;
	UVCoord.X = (MinimapRelative.X + ViewRange) / (2.0f * ViewRange);
	UVCoord.Y = 1.0f - (MinimapRelative.Y + ViewRange) / (2.0f * ViewRange); // 翻转Y轴
	
	return UVCoord;
}