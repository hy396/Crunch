// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_ArcaneOrb.generated.h"

class AArcaneOrbProjectile;

/**
 * 奥术弹技能（法师 Q）
 * 发射慢速穿透奥术能量球，穿透所有敌人造成魔法伤害
 * 飞行距离越远伤害越高，命中敌人回复少量法力
 */
UCLASS()
class CRUNCH_API UGA_ArcaneOrb : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_ArcaneOrb();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 施法动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> CastMontage;

	// 穿透弹投射物类
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AArcaneOrbProjectile> OrbProjectileClass;

	// 投射物速度（故意偏慢）
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float OrbSpeed = 1200.f;

	// 最大飞行距离
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float OrbMaxDistance = 3500.f;

	// 伤害效果（魔法伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGenericDamageEffectDef DamageEffect;

	// 法力回复效果（命中时自身回蓝）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> ManaRestoreEffect;

	// 距离伤害倍率（最远处倍率）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float DistanceDamageMultiplier = 1.5f;

	// 发射手部Socket名称
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName CastSocketName = "Hand_R";

	// 奥术弹视觉提示标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag ArcaneOrbCueTag;

	// 施法动画事件回调
	UFUNCTION()
	void OnCast(FGameplayEventData Payload);
};
