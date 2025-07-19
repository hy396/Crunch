// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Player/NumberPopComponent_NiagaraText.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddNiagaraText(const FNumberPopRequest& NewRequest);
};
