// 幻雨喜欢小猫咪


#include "RenderActor.h"

#include "Components/SceneCaptureComponent2D.h"


// Sets default values
ARenderActor::ARenderActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 创建场景锚点作为根组件
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	// 创建场景捕获组件并添加到根组件
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture Component");
	CaptureComponent->SetupAttachment(RootComp);

	// 配置捕获参数
	CaptureComponent->bCaptureEveryFrame = false;	// 禁用自动捕获，手动控制
	CaptureComponent->FOVAngle = 30.f;				// 窄视角适合头部特写
}

void ARenderActor::SetRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	CaptureComponent->TextureTarget = RenderTarget;
}

void ARenderActor::UpdateRender()
{
	if (CaptureComponent)
	{
		// 捕获场景
		CaptureComponent->CaptureScene();
	}
}

// Called when the game starts or when spawned
void ARenderActor::BeginPlay()
{
	Super::BeginPlay();

	// 只显示当前Actor的组件到捕获画面
	CaptureComponent->ShowOnlyActorComponents(this);

	// 将Actor移动到远离主场景的位置，避免干扰
	SetActorLocation(FVector{0.f, 0.f, 100000.f});
}

