// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Freeze.generated.h"

/**
 * 冰冻技能：角色瞄准地面位置释放冰冻效果，对区域内敌人造成伤害
 */
UCLASS()
class UGA_Freeze : public UCGameplayAbility
{
	GENERATED_BODY()
public:	
	UGA_Freeze();
	// 激活技能
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 目标选择半径（效果作用范围）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingRadius = 1000.f;

	// 目标选择距离（最大施法距离）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 2000.f;

	// 地面目标选择器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;

	// 瞄准阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FreezeTargetingMontage;

	// 施法阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FreezeCastMontage;

	// 伤害效果（冰冻伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGenericDamageEffectDef DamageEffect;

	// TODO:没用上,使用了GE应用GC的方式应用上去了
	// 目标位置视觉提示标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag FreezingTargetGameplayCueTag;

	// 冰冻效果视觉标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag FreezingGameplayCueTag;
	
	// 目标选择完成回调
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 目标选择取消回调
	UFUNCTION()
	void TargetingCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};