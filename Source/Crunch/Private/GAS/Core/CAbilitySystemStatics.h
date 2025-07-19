// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CAbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class UCAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FGameplayTag GetBasicAttackAbilityTag();
	// 获取摄像机震动游戏事件标签
	static FGameplayTag GetCameraShakeGameplayCueTag();

	static FGameplayTag GetDamageNumberGameplayCueTag();

	// 获取技能冷却
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability);
	// 获取技能消耗
	static float GetStaticCostForAbility(const UGameplayAbility* Ability);
};
