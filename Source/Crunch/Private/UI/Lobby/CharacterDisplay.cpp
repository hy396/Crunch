// 幻雨喜欢小猫咪


#include "CharacterDisplay.h"

#include "Camera/CameraComponent.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACharacterDisplay::ACharacterDisplay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 创建根组件（场景组件）
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Comp"));

	// 创建模型组件
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Component");
	MeshComponent->SetupAttachment(RootComponent);

	// 创建摄像机组件并附加到根组件
	ViewCameraComponent = CreateDefaultSubobject<UCameraComponent>("View Camera Component");
	ViewCameraComponent->SetupAttachment(GetRootComponent());
}

void ACharacterDisplay::ConfigureWithCharacterDefinition(const UPDA_CharacterDefinition* CharacterDefinition)
{
	// 安全检查
	if (!CharacterDefinition) return;
	MeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	// 加载并设置角色展示网格
	MeshComponent->SetSkeletalMesh(CharacterDefinition->LoadDisplayMesh());
	
	// 配置动画模式为蓝图驱动
	MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
	// 加载并设置展示动画蓝图
	MeshComponent->SetAnimClass(CharacterDefinition->LoadDisplayAnimationBP());
	
	if (UAnimMontage* IntroAnimation = CharacterDefinition->LoadCharacterIntroAnimation())
	{
		UAnimInstance* OwnerAnimInst = MeshComponent->GetAnimInstance();
		if (OwnerAnimInst && !OwnerAnimInst->Montage_IsPlaying(IntroAnimation))
		{
			OwnerAnimInst->Montage_Play(IntroAnimation);
		}
	}

}



