// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "DamageTextComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// 设置货币文字（金币或经验）
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrencyText(int32 Amount, ECurrencyType CurrencyType);

};
