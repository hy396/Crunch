// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_MeteorStrike.generated.h"

class ATargetActor_GroundPick;
class ATargetActor_Around;
class ABurningGroundActor;

/**
 * 陨石打击技能（法师 F）
 * 选择地面位置召唤陨石从天而降
 * 落点造成大范围魔法伤害+击退
 * 留下持续燃烧区域（5秒）对站在其中的敌人造成灼烧伤害
 */
UCLASS()
class CRUNCH_API UGA_MeteorStrike : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_MeteorStrike();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 瞄准阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> TargetingMontage;

	// 施法动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> CastMontage;

	// 地面目标选择器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	// 落地范围检测器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Around> ImpactTargetActorClass;

	// 落点范围半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 500.f;

	// 施法射程
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float CastRange = 2500.f;

	// 落地伤害效果（魔法伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef ImpactDamageEffect;

	// 落地击退速度
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ImpactPushSpeed = 3000.f;

	// 瞄准状态GE
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> AimEffect;
	FActiveGameplayEffectHandle AimEffectHandle;

	// 燃烧地面Actor类
	UPROPERTY(EditDefaultsOnly, Category = "BurningGround")
	TSubclassOf<ABurningGroundActor> BurningGroundActorClass;

	// 燃烧区域持续时间
	UPROPERTY(EditDefaultsOnly, Category = "BurningGround")
	float BurnZoneDuration = 5.f;

	// 燃烧伤害间隔
	UPROPERTY(EditDefaultsOnly, Category = "BurningGround")
	float BurnTickInterval = 0.5f;

	// 燃烧伤害GE
	UPROPERTY(EditDefaultsOnly, Category = "BurningGround")
	TSubclassOf<UGameplayEffect> BurnDamageEffect;

	// 陨石下落延迟（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float MeteorFallDelay = 1.0f;

	// 陨石落地视觉标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag MeteorImpactCueTag;

	// 缓存的蒙太奇任务
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* TargetingMontageTask;

	// 缓存的落点位置
	FVector CachedImpactLocation;

	// 定时器句柄
	FTimerHandle MeteorFallTimerHandle;

	// 目标确认回调
	UFUNCTION()
	void OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 目标取消回调
	UFUNCTION()
	void OnTargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 陨石落地
	void OnMeteorImpact();

	// 落地范围检测回调
	UFUNCTION()
	void OnImpactTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 添加/移除瞄准状态
	void AddAimEffect();
	void RemoveAimEffect();
};
