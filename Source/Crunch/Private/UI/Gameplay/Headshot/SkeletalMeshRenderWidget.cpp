// 幻雨喜欢小猫咪


#include "SkeletalMeshRenderWidget.h"

#include "SkeletalMeshRenderActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "UI/Common/Rendering/RenderActorTargetInterface.h"

void USkeletalMeshRenderWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 获取拥有该UI的玩家角色
	ACharacter* PlayerCharacter = GetOwningPlayerPawn<ACharacter>();
	// 尝试转换为渲染目标接口
	IRenderActorTargetInterface* PlayerCharacterRenderTargetInterface = Cast<IRenderActorTargetInterface>(PlayerCharacter);

	if (PlayerCharacter && SkeletalMeshRenderActor)
	{
		// 配置骨骼网格和动画类
		SkeletalMeshRenderActor->ConfigureSkeletalMesh(
			PlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), 
			PlayerCharacter->GetMesh()->GetAnimClass()
		);

		// 获取场景捕捉组件， 这个看着也不错，把组件放角色里，甚至还能减少新的Actor创建，就是过于之大
		// USceneCaptureComponent2D* SceneCaptureComponent = GetOwningPlayerPawn<ACharacter>()->GetComponentByClass<USceneCaptureComponent2D>();

		// 获取场景捕捉组件
		USceneCaptureComponent2D* SceneCapture = SkeletalMeshRenderActor->GetCaptureComponent();
		// 如果接口和场景捕捉组件都有效
		if (PlayerCharacterRenderTargetInterface && SceneCapture)
		{
			// 设置捕捉组件的相对位置和旋转
			SceneCapture->SetRelativeLocation(PlayerCharacterRenderTargetInterface->GetCaptureLocalPosition());
			SceneCapture->SetRelativeRotation(PlayerCharacterRenderTargetInterface->GetCaptureLocalRotation());
		}
	}
}

void USkeletalMeshRenderWidget::SpawnRenderActor()
{
	if (!SkeletalMeshRenderActorClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 设置生成参数
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 生成渲染Actor
	SkeletalMeshRenderActor = World->SpawnActor<ASkeletalMeshRenderActor>(SkeletalMeshRenderActorClass, SpawnParams);
}

ARenderActor* USkeletalMeshRenderWidget::GetRenderActor() const
{
	return SkeletalMeshRenderActor;
}
