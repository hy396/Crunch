// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
protected:

	// 获取当前玩家控制器
	UFUNCTION(BlueprintPure)
	APlayerController* GetOwningFrontendPlayerController();

private:
	TWeakObjectPtr<APlayerController> CachedOwningFrontendPC;
	
};
