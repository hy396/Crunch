// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GAP_Dead.generated.h"

/**
 * 死亡能力类
 * 负责角色死亡时的奖励分配（经验、金币等）
 */
UCLASS()
class CRUNCH_API UGAP_Dead : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGAP_Dead();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 2026/1/28 New Begin~
	// 连杀系统赏金配置
	UPROPERTY(EditDefaultsOnly, Category = "Bounty", meta=(DisplayName="基础赏金"))
	float BaseBounty = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bounty", meta=(DisplayName="每层连杀增加金币"))
	float BountyPerStreak = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bounty", meta=(DisplayName="每层连败扣除金币"))
	float PenaltyPerDeath = 40.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bounty", meta=(DisplayName="最低赏金保底"))
	float MinBounty = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bounty", meta=(DisplayName="最高赏金封顶"))
	float MaxBounty = 600.f;

	// 2026/1/28 End~

	// 奖励分配的范围，在该范围内的队友可获得奖励
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="奖励范围"))
	float RewardRange = 1000.f;
	// 基础经验奖励
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="基础经验奖励"))
	float BaseExperienceReward = 200.f;

	// 基础金币奖励
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="基础金币奖励"))
	float BaseGoldReward = 200.f;

	// 额外经验奖励系数（根据目标经验值加成）
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="经验奖励系数（每单位经验）"))
	float ExperienceRewardPerExperience = 0.1f;

	// 额外金币奖励系数（根据目标经验值加成）
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="金币奖励系数（每单位经验）"))
	float GoldRewardPerExperience = 0.05f;

	// 击杀者奖励占比（击杀者获得的奖励比例，其余分配给队友）
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="击杀者奖励比例（0-1）"))
	float KillerRewardPortion = 0.5f;
	
	// 获取奖励目标（如附近的队友等）
	TArray<AActor*> GetRewardTargets() const;

	// 奖励GE（用于发放经验、金币等）
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="奖励效果（GE类）"))
	TSubclassOf<UGameplayEffect> RewardEffect;
	
	// 击杀英雄GE
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="击杀英雄效果（GE类）"))
	TSubclassOf<UGameplayEffect> KillHeroEffect;
	
	// 死亡GE
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="死亡效果（GE类）"))
	TSubclassOf<UGameplayEffect> DeadEffect;

	// 助攻GE
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="助攻效果（GE类）"))
	TSubclassOf<UGameplayEffect> AssistEffect;
	
	// 补兵GE
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="补兵效果（GE类）"))
	TSubclassOf<UGameplayEffect> LastHitEffect;

	// 连杀增加 GE
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="连杀增加效果（GE类）"))
	TSubclassOf<UGameplayEffect> KillStreakIncreaseEffect;

	// 连败增加 GE
	UPROPERTY(EditDefaultsOnly, Category = "Reward", meta=(DisplayName="连败增加效果（GE类）"))
	TSubclassOf<UGameplayEffect> DeathStreakIncreaseEffect;

};
