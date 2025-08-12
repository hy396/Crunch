// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RenderActorTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URenderActorTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 角色渲染目标接口
 * 用于获取角色渲染时的捕获位置和旋转（如头像渲染、预览等）
 */
class IRenderActorTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 获取捕获的本地位置
	virtual FVector GetCaptureLocalPosition() const = 0;

	// 获取捕获的本地旋转
	virtual FRotator GetCaptureLocalRotation() const = 0;
};
