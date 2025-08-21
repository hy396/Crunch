// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UCameraComponent;
class UPDA_CharacterDefinition;

/**
 * 角色展示Actor - 用于在角色选择界面中展示3D角色模型
 */
UCLASS()
class CRUNCH_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()

public:	
	// 构造函数
	ACharacterDisplay();
	
	/**
	 * 配置角色展示
	 * @param CharacterDefinition 角色定义数据资产，包含要展示的模型和动画信息
	 */
	void ConfigureWithCharacterDefinition(const UPDA_CharacterDefinition* CharacterDefinition);
protected:
	// 角色网格组件 - 用于显示角色模型
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Character Display")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
private:	

	// 摄像机组件 - 用于控制角色展示的视角
	UPROPERTY(VisibleDefaultsOnly, Category = "Character Display")
	TObjectPtr<UCameraComponent> ViewCameraComponent;
};
