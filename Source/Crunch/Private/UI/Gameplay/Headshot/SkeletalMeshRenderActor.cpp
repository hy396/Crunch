// 幻雨喜欢小猫咪


#include "SkeletalMeshRenderActor.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASkeletalMeshRenderActor::ASkeletalMeshRenderActor()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	MeshComp->SetupAttachment(GetRootComponent());

	// 禁用碰撞
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 设置光照通道，仅使用第二通道（通常用于UI渲染优化）
	MeshComp->SetLightingChannels(false, true, false);
}

void ASkeletalMeshRenderActor::ConfigureSkeletalMesh(USkeletalMesh* MeshAsset, TSubclassOf<UAnimInstance> AnimBlueprint)
{
	// 设置骨骼网格资源
	MeshComp->SetSkeletalMeshAsset(MeshAsset);

	// 设置动画蓝图
	MeshComp->SetAnimInstanceClass(AnimBlueprint);
}

// Called when the game starts or when spawned
void ASkeletalMeshRenderActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 仅在场景捕获时可见（用于UI渲染，不显示在主场景）
	MeshComp->SetVisibleInSceneCaptureOnly(true);
}


