// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_AxeThrow.generated.h"

class ABoomerangAxeProjectile;

/**
 * 掷斧回旋技能（大斧战士 F）
 * 投掷巨斧，沿直线飞出后回旋返回
 * 去程和回程均造成物理伤害并减速
 * 标志性技能
 */
UCLASS()
class CRUNCH_API UGA_AxeThrow : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_AxeThrow();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 投掷动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ThrowMontage;

	// 接住动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> CatchMontage;

	// 回旋斧投射物类
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ABoomerangAxeProjectile> AxeProjectileClass;

	// 投掷速度
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ThrowSpeed = 2500.f;

	// 最大飞行距离
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float MaxThrowDistance = 2000.f;

	// 伤害效果（物理伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef DamageEffect;

	// 减速效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> SlowEffect;

	// 发射Socket名称
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName ThrowSocketName = "Hand_R";

	// 掷斧视觉提示标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag AxeThrowCueTag;

	// 投掷动画事件回调
	UFUNCTION()
	void OnRelease(FGameplayEventData Payload);

	// 斧头返回事件回调
	UFUNCTION()
	void OnAxeReturned(FGameplayEventData Payload);
};
