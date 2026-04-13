// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_WarCry.generated.h"

class ATargetActor_Around;

/**
 * 战吼技能（大斧战士 Q）
 * 发出震天怒吼，对周围敌人造成物理伤害，降低其护甲并施加减速效果
 * 短冷却核心循环技能
 */
UCLASS()
class CRUNCH_API UGA_WarCry : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_WarCry();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 战吼动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> WarCryMontage;

	// 圆形范围检测器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Around> TargetActorClass;

	// 检测半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float DetectionRadius = 600.f;

	// 伤害效果（物理伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef DamageEffect;

	// 减甲减速Debuff效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DebuffEffect;

	// 轻微击退速度
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float PushSpeed = 500.f;

	// 战吼视觉提示标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag WarCryCueTag;

	// 动画事件回调：吼叫时刻触发伤害
	UFUNCTION()
	void OnShout(FGameplayEventData Payload);

	// 目标检测完成回调
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
