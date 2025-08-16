// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_BlackHole.generated.h"

/**
 * 黑洞技能：分三阶段执行
 * 1. 目标选择阶段：玩家选择黑洞放置位置
 * 2. 黑洞维持阶段：黑洞持续吸引周围目标
 * 3. 最终爆炸阶段：引爆黑洞造成伤害和击退效果
 */
UCLASS()
class CRUNCH_API UGA_BlackHole : public UCGameplayAbility
{
	GENERATED_BODY()
public:	
	// 技能激活时调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// 技能结束时调用（清理资源）
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 目标区域半径（黑洞影响范围）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000.f;

	// 黑洞吸引力大小
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackHolePullSpeed = 3000.f;

	// 目标选择射线距离
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	// 目标检测频率（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingInterval = 0.3f;

	// 黑洞持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackHoleDuration = 6.f;

	// 目标选择阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> TargetingMontage;

	// 黑洞释放阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HoldBlackHoleMontage;

	// 最终爆炸阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FinalBlowMontage;

	// 瞄准标签的GE，用于添加瞄准标签
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> AimEffect;
	FActiveGameplayEffectHandle AimEffectHandle; // 效果句柄

	// // 黑洞维持阶段的效果（如角色专注状态）
	// UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	// TSubclassOf<UGameplayEffect> FocusEffect;
	// FActiveGameplayEffectHandle FocusEffectHandle; // 效果句柄

	// 地面目标选择器（用于选择黑洞位置）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;
	
	// 黑洞目标选择器（持续吸引目标）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_BlackHole> BlackHoleTargetActorClass;

	// 最终爆炸伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGenericDamageEffectDef FinalBlowDamageEffect;

	// 爆炸击退力度
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BlowPushSpeed = 3000.f;

	// 最终爆炸特效标签
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGameplayTag FinalBlowCueTag;
	
	// 动画任务引用（需用UPROPERTY防止垃圾回收）
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* PlayCastBlackHoleMontageTask;
	UPROPERTY()
	class UAbilityTask_WaitTargetData* BlackHoleTargetingTask;

	// 放置黑洞位置回调
	UFUNCTION()
	void PlaceBlackHole(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	// 放置取消回调
	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	// 接收最终爆炸目标回调
	UFUNCTION()
	void FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 应用/移除 瞄准状态
	void AddAimEffect();
	void RemoveAimEffect();

	// 应用/移除 聚焦状态
	// void AddFocusEffect();
	// void RemoveFocusEffect();
	// 目标数据接收回调（处理命中目标）
	
	// 命中时应用的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGenericDamageEffectDef BlackHoleHitEffectDef;
	
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
