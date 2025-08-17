// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TNetStatics.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UTNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:	
	/**
	 * 获取每支队伍的基础玩家数量
	 * @return 玩家数量（默认值为5）
	 */
	static uint8 GetPlayerCountPerTeam();
};
