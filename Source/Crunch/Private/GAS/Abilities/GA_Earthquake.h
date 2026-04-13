// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Earthquake.generated.h"

class ATargetActor_Around;

/**
 * 地裂技能（大斧战士 R 大招）
 * 跳至空中后三次砸地，每次范围递增
 * 第1/2次：中等伤害+眩晕
 * 第3次：重伤害+击飞+击退
 * 释放期间无敌+控制免疫
 */
UCLASS()
class CRUNCH_API UGA_Earthquake : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Earthquake();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 地裂完整动画蒙太奇（包含3次砸地分段）
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> EarthquakeMontage;

	// 圆形检测器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Around> TargetActorClass;

	// 三次砸地的范围
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Slam1Radius = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Slam2Radius = 600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Slam3Radius = 900.f;

	// 三次砸地的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef Slam1DamageEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef Slam2DamageEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef Slam3DamageEffect;

	// 眩晕效果（第1/2次砸地）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> StunEffect;

	// 击飞效果（第3次砸地）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> KnockupEffect;

	// 第3次砸地的击退速度
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float Slam3PushSpeed = 4000.f;

	// 起跳推力
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float LeapUpSpeed = 1500.f;

	// 地裂视觉标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag EarthquakeCueTag;

	// 当前砸地索引（内部状态）
	int32 CurrentSlamIndex = 0;

	// 砸地动画事件回调
	UFUNCTION()
	void OnSlam1(FGameplayEventData Payload);
	UFUNCTION()
	void OnSlam2(FGameplayEventData Payload);
	UFUNCTION()
	void OnSlam3(FGameplayEventData Payload);

	// 通用砸地检测逻辑
	void ExecuteSlam(float Radius, const FGenericDamageEffectDef& DamageEffect, TSubclassOf<UGameplayEffect> CCEffect, bool bPushTargets);

	// 砸地范围检测回调
	UFUNCTION()
	void OnSlamTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 缓存当前砸地的参数
	FGenericDamageEffectDef CurrentSlamDamage;
	TSubclassOf<UGameplayEffect> CurrentCCEffect;
	bool bCurrentSlamPushTargets = false;
};
