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

	// 判断Actor是否死亡
	static bool IsActorDead(const AActor* ActorToCheck);
	// 判断是否为英雄
	static bool IsHero(const AActor* ActorToCheck);
	// 判断Actor是否拥有指定标签
	static bool ActorHasTag(const AActor* ActorToCheck, const FGameplayTag& Tag);
	// 判断技能是否达到最大等级
	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec, const float PlayLevel);

	// 获取技能冷却
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability);
	// 获取技能消耗
	static float GetStaticCostForAbility(const UGameplayAbility* Ability);


	
	// 检查当前是否可以支付技能消耗（法力等）
	// @param AbilitySpec - 要检查的技能规格数据
	// @param ASC - 所属的能力系统组件
	// @return 如果资源足够支付消耗返回true，否则false
	static bool CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC);

	// 检查技能消耗（静态）
	static bool CheckAbilityCostStatic(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);

	// 获取技能的当前法力消耗值
	// @param AbilityCDO - 技能的默认对象（Class Default Object）
	// @param ASC - 所属的能力系统组件（用于获取属性修饰符）
	// @param AbilityLevel - 当前技能等级
	// @return 计算后的实际法力消耗值
	static float GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	
	// 获取技能的当前冷却时间
	// @param AbilityCDO - 技能的默认对象
	// @param ASC - 所属的能力系统组件（用于获取冷却修饰符）
	// @param AbilityLevel - 当前技能等级
	// @return 计算后的实际冷却时间（秒）
	static float GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);

	// 获取技能的当前冷却时间
	// @param AbilityCDO - 技能的默认对象
	// @param ASC - 所属的能力系统组件（用于获取冷却修饰符）
	// @param AbilityLevel - 当前技能等级
	// @return 计算后的实际冷却时间（秒）
	static float GetCooldownDurationForMMCCD(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	
	// 获取技能的剩余冷却时间
	// @param AbilityCDO - 技能的默认对象
	// @param ASC - 所属的能力系统组件
	// @return 剩余的冷却时间（秒），如果不在冷却中返回0
	static float GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);

};
