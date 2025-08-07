// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/Rendering/RenderActorWidget.h"
#include "SkeletalMeshRenderWidget.generated.h"

class ASkeletalMeshRenderActor;
/**
 * 用于在UI中渲染骨骼网格体的Widget
 * 负责生成和管理骨骼网格渲染Actor
 */
UCLASS()
class CRUNCH_API USkeletalMeshRenderWidget : public URenderActorWidget
{
	GENERATED_BODY()
public:
	// 构建时回调（初始化控件和渲染Actor）
	virtual void NativeConstruct() override;

private:
	// 生成骨骼网格渲染Actor实例
	virtual void SpawnRenderActor() override; 

	// 获取骨骼网格渲染Actor实例
	virtual ARenderActor* GetRenderActor() const override;
	
	// 骨骼网格渲染Actor的类（可在编辑器中指定）
	UPROPERTY(EditDefaultsOnly, Category = "SKeletal Mesh Render")
	TSubclassOf<ASkeletalMeshRenderActor> SkeletalMeshRenderActorClass;

	// 当前生成的骨骼网格渲染Actor实例
	UPROPERTY()
	TObjectPtr<ASkeletalMeshRenderActor> SkeletalMeshRenderActor;
};
