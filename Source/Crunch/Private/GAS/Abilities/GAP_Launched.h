// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GAP_Launched.generated.h"

/**
 * 被击飞能力类
 * 用于处理角色被击飞时的特殊能力逻辑
 */
UCLASS()
class UGAP_Launched : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	// 构造函数
	UGAP_Launched();

	// 激活能力时调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 获取击飞激活事件Tag
	static FGameplayTag GetLaunchedAbilityActivationTag();
};
