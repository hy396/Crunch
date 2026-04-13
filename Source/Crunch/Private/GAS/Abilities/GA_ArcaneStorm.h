// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_ArcaneStorm.generated.h"

class ATargetActor_GroundPick;
class ATargetActor_BlackHole;

/**
 * 奥术风暴技能（法师 R 大招）
 * 选择地面区域召唤持续5秒的奥术风暴
 * 风暴区域内每0.5秒造成魔法伤害+减速
 * 风暴结束时爆发大量伤害并沉默区域内所有敌人2秒
 * 法师引导期间移速降低
 */
UCLASS()
class CRUNCH_API UGA_ArcaneStorm : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_ArcaneStorm();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 瞄准动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> TargetingMontage;

	// 引导动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ChannelMontage;

	// 地面选择器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	// 风暴区域检测器类（复用BlackHole的周期性检测，PullSpeed=0）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_BlackHole> StormTargetActorClass;

	// 风暴半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float StormRadius = 800.f;

	// 施法射程
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float CastRange = 2000.f;

	// 检测间隔
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TickInterval = 0.5f;

	// 风暴持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float StormDuration = 5.f;

	// 每次Tick的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef TickDamageEffect;

	// 最终爆发伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef FinalBurstDamageEffect;

	// 周期性减速效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> SlowEffect;

	// 最终爆发沉默效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> SilenceEffect;

	// 自身减速效果（引导期间）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> SelfSlowEffect;
	FActiveGameplayEffectHandle SelfSlowEffectHandle;

	// 瞄准状态GE
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> AimEffect;
	FActiveGameplayEffectHandle AimEffectHandle;

	// 风暴视觉标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag StormCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag StormExplosionCueTag;

	// 缓存的蒙太奇任务
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* TargetingMontageTask;

	// 缓存的风暴检测任务
	UPROPERTY()
	class UAbilityTask_WaitTargetData* StormTargetingTask;

	// 缓存风暴位置
	FVector StormLocation;

	// 目标确认回调
	UFUNCTION()
	void OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 目标取消回调
	UFUNCTION()
	void OnTargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 风暴周期性伤害回调
	UFUNCTION()
	void OnStormTick(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 风暴结束爆发回调
	UFUNCTION()
	void OnStormEnd(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void AddAimEffect();
	void RemoveAimEffect();
};
