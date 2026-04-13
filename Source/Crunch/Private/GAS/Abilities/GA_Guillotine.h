// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Guillotine.generated.h"

class ATargetActor_Line;

/**
 * 断头台技能（大斧战士 E）
 * 蓄力后前方锥形重劈，对低血量目标造成额外伤害（斩杀效果）
 * 命中目标附带流血DOT
 */
UCLASS()
class CRUNCH_API UGA_Guillotine : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Guillotine();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 劈砍动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> GuillotineMontage;

	// 线性检测器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Line> TargetActorClass;

	// 劈砍射程
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float CleaveRange = 400.f;

	// 劈砍检测半径（宽度）
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float CleaveRadius = 150.f;

	// 基础伤害效果（物理伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef DamageEffect;

	// 斩杀额外伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef ExecuteBonusDamage;

	// 流血效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> BleedEffect;

	// 斩杀血量阈值（百分比，低于此值触发额外伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float ExecuteHealthThreshold = 0.3f;

	// 劈砍视觉提示标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag GuillotineCueTag;

	// 动画事件：劈砍落地时触发伤害
	UFUNCTION()
	void OnSlam(FGameplayEventData Payload);

	// 目标检测完成回调
	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
};
