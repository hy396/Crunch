// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/Rendering/RenderActor.h"
#include "SkeletalMeshRenderActor.generated.h"

/**
 * 用于在UI中渲染骨骼网格体的Actor
 * 继承自通用渲染Actor，可设置骨骼网格和动画蓝图
 */
UCLASS()
class CRUNCH_API ASkeletalMeshRenderActor : public ARenderActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkeletalMeshRenderActor();
	// 配置骨骼网格和动画蓝图
	void ConfigureSkeletalMesh(USkeletalMesh* MeshAsset, TSubclassOf<UAnimInstance> AnimBlueprint);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 渲染的骨骼网格体组件
	UPROPERTY(VisibleAnywhere, Category = "Skeletal Mesh Renderer")
	TObjectPtr<USkeletalMeshComponent> MeshComp;
};
