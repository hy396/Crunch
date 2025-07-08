// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UAnimInstance* GetOwnerAnimInstance() const;
	// 毫无用武之地
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = true, bool bIgnoreSelf = true) const;

	/**
	* @brief 向命中结果中的 Actor 应用指定的 GameplayEffect
	*
	* 根据传入的 HitResult 和 Effect 类型创建 EffectSpec，并应用到目标 Actor。
	* 适用于攻击判定、技能释放等需要即时生效效果的场景。
	* 
	* @param HitResult 命中信息，包含目标 Actor 和碰撞数据
	* @param GameplayEffect 要应用的 GameplayEffect 类型
	* @param Level 效果等级，默认为 1，用于控制伤害/增益强度
	*/
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
};
