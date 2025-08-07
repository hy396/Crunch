// 幻雨喜欢小猫咪


#include "RenderActorWidget.h"

#include "RenderActor.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"

void URenderActorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	// 设置大小
	RenderSizeBox->SetWidthOverride(RenderSize.X);
	RenderSizeBox->SetHeightOverride(RenderSize.Y);
}

void URenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 创建特定类型的渲染Actor（由子类实现）
	SpawnRenderActor();
	
	// 配置渲染Actor和渲染目标
	ConfigureRenderActor();
	
	// 启动渲染循环
	BeginRenderCapture();
}

void URenderActorWidget::BeginDestroy()
{
	// 停止渲染循环（避免内存泄漏）
	StopRenderCapture();
	Super::BeginDestroy();
}

void URenderActorWidget::ConfigureRenderActor()
{
	if (!GetRenderActor())
	{
		UE_LOG(LogTemp, Error, TEXT("没有渲染Actor，将不会渲染任何内容"));
		return;
	}

	// 创建渲染目标纹理
	RenderTarget = NewObject<UTextureRenderTarget2D>(this);

	// 设置格式和尺寸
	RenderTarget->InitAutoFormat(static_cast<uint32>(RenderSize.X), static_cast<uint32>(RenderSize.Y));
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8_SRGB; // 使用sRGB格式保证颜色准确

	// 将渲染目标设置给Actor
	GetRenderActor()->SetRenderTarget(RenderTarget);

	// 获取显示图像的动态材质实例
	UMaterialInstanceDynamic* DisplayImageDynamicMaterial = DisplayImage->GetDynamicMaterial();

	if (DisplayImageDynamicMaterial)
	{
		// 绑定渲染目标纹理
		DisplayImageDynamicMaterial->SetTextureParameterValue(DisplayImageRenderTargetParamName, RenderTarget);
	}
}

void URenderActorWidget::BeginRenderCapture()
{
	// 计算渲染间隔（秒/帧）
	RenderTickInterval = 1.f / static_cast<float>(FrameRate);
	
	// 获取世界上下文
	UWorld* World = GetWorld();
	if (World)
	{
		// 设置定时器按指定帧率调用渲染更新
		World->GetTimerManager().SetTimer(
			RenderTimerHandle, 
			this, 
			&URenderActorWidget::UpdateRender,
			RenderTickInterval, 
			true // 循环执行
		);
	}
}

void URenderActorWidget::UpdateRender()
{
	// 安全检查后执行渲染更新
	if (GetRenderActor())
	{
		GetRenderActor()->UpdateRender();
	}
}

void URenderActorWidget::StopRenderCapture()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// 停止定时器
		World->GetTimerManager().ClearTimer(RenderTimerHandle);
	}
}
