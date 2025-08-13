// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Laser.generated.h"

class ATargetActor_Line;
/**
 * 激光类技能：
 * - 持续施法技能，消耗法力值维持激光
 * - 对路径上的目标造成持续伤害和击退效果
 */
UCLASS()
class CRUNCH_API UGA_Laser : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	// 技能激活时调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// 技能结束时调用
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 目标检测范围（厘米）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 4000;

	// 激光检测半径（厘米）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float DetectionCylinderRadius = 30.f;

	// 目标检测频率（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingInterval = 0.3f;

	// 命中时应用的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef HitDamageEffect;

	// 持续施法时的法力消耗效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> OnGoingConsumptionEffect;

	// 持续消耗效果的激活句柄
	FActiveGameplayEffectHandle OnGoingConsumptionEffectHandle;
	
	// 激光技能动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TObjectPtr<UAnimMontage> LaserMontage;

	// 目标Actor附加的骨骼名称
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName TargetActorAttachSocketName = "Laser";

	// 激光目标Actor类（直线检测）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Line> LaserTargetActorClass;

	// 动画事件触发的射击回调
	UFUNCTION()
	void ShootLaser(FGameplayEventData Payload);
	
	// 法力值更新回调（用于检测法力不足）
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);

	// 目标数据接收回调（处理命中目标）
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
