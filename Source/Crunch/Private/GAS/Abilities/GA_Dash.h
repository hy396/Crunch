// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Dash.generated.h"

class UCharacterMovementComponent;
/**
 * 冲刺能力类，使角色能够向目标方向冲刺并对路径上的敌人造成伤害
 */
UCLASS()
class CRUNCH_API UGA_Dash : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	// 激活能力时调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// 结束能力时调用
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 冲刺动作的动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TObjectPtr<UAnimMontage> DashMontage;

	// 目标检测半径（单位：厘米）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetDetectionRadius = 300.f;

	// 本地游戏提示标签（用于视觉效果）
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag LocalGameplayCueTag;

	// 目标检测器附加的骨骼名称
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName TargetActorAttachSocketName = "TargetDashCenter";

	// 目标检测器类（圆形范围检测）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_Around> TargetActorClass;

	// 命中目标后的击退速度
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float TargetHitPushSpeed = 3000.f;

	// 命中目标时应用的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef DamageEffect;

	// 冲刺过程中应用的持续效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DashEffect;
	
	// 当前激活的冲刺效果句柄
	FActiveGameplayEffectHandle DashEffectHandle;

	// 推动角色前进的定时器句柄
	FTimerHandle PushForwardInputTimerHandle;

	// 推动角色沿当前方向前进
	void PushForward();
	
	// 缓存角色移动组件
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	// 动画事件触发时开始冲刺逻辑
	UFUNCTION()
	void StartDash(FGameplayEventData Payload);

	// 目标检测完成回调
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
