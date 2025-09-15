// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
// #include "Abilities/Tasks/AbilityTask_MoveToLocation.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_ChainAttack.generated.h"

/**
 * 连锁攻击技能
 * 选取范围内的目标，使用运动扭曲依次攻击每个目标
 */
UCLASS()
class CRUNCH_API UGA_ChainAttack : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_ChainAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// 结束技能
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	// 蓄力动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Animation")
	TObjectPtr<UAnimMontage> ChargeMontage;
	// 突进动画蒙太奇片段
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Animation")
	FName DashName = "Loop";
	// 最后一击动画蒙太奇片段
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Animation")
	FName LastHitName = "LastHit";
	// // 攻击动画蒙太奇
	// UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Animation")
	// TObjectPtr<UAnimMontage> AttackMontage;
	//
	// // 突进动画蒙太奇
	// UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Animation")
	// TObjectPtr<UAnimMontage> DashMontage;

	// 目标检测半径（单位：厘米）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetDetectionRadius = 300.f;
	
	// 本地游戏提示标签（用于视觉效果）
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag LocalGameplayCueTag;

	// 本地游戏提示标签（用于视觉效果）
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag AttackLocalGameplayCueTag;
	
	// 目标检测器附加的骨骼名称
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName TargetActorAttachSocketName = "TargetDashCenter";

	// 目标检测器类（圆形范围检测）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_Around> TargetActorClass;
	
	// 命中目标后的击退速度
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float TargetHitPushSpeed = 3000.f;
	
	// 突进伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Damage")
	FGenericDamageEffectDef DamageEffect;

	// 最后一击伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Damage")
	FGenericDamageEffectDef LastHitDamageEffect;
private:
	// 当前目标列表
	TArray<TWeakObjectPtr<AActor>> TargetActors;

	// 当前攻击索引
	int32 CurrentAttackIndex = 0;
	// 蓄力持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Charge")
	float ChargeDuration = 1.f;
	
	// 攻击间隔
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Attack")
	float AttackInterval = 0.5f;

	// 定时器句柄
	FTimerHandle ChainAttackTimerHandle;

	// 索敌范围
	UPROPERTY(EditDefaultsOnly, Category = "Chain Attack|Target")
	float TargetRange = 3000.f;


	// 目标检测完成回调
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 计算中心点
	FVector CalculateCenter();

	// 缓存中心点
	FVector Center;
	
	// 检测范围内的敌人
	void DetectTargets();

	// 开始攻击定时器
	UFUNCTION()
	void StartAttackTimer(FGameplayEventData Payload);
	// 执行单次攻击
	void ExecuteSingleAttack();

	// 全面采取运动扭曲来实现位移行为，因此抛弃了移动任务
	// 防止GC，需要UPROPERTY
	// UPROPERTY()
	// UAbilityTask_MoveToLocation* MoveTask = nullptr;

	// 飞过去了
	// UFUNCTION()
	// void OnReachedCenter();
	// 处理最后一击伤害
	// void HandleLastHitDamage();
	
	// 执行最后一击范围伤害
	UFUNCTION()
	void ExecuteLastHit();
	UFUNCTION()
	void ExecuteLastHitDamage(FGameplayEventData Payload);
	// 停止攻击
	UFUNCTION()
	void StopAttacks();

	
};
