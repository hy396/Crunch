// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Blink.generated.h"

/**
 * 闪烁技能：角色选择目标位置进行传送，落地时对周围敌人造成伤害和击退
 */
UCLASS()
class CRUNCH_API UGA_Blink : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Blink();
	// 激活技能
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 瞄准阶段动画（选择目标位置）
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TObjectPtr<UAnimMontage> TargetingMontage;

	// 传送阶段动画
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TObjectPtr<UAnimMontage> TeleportMontage;


	// 地面目标选择器类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_GroundPick> GroundPickTargetActorClass;

	// 落地伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGenericDamageEffectDef DamageEffect;

	// 落地击退速度
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BlinkLandTargetPushSpeed = 5000.f;

	// 落地作用范围半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000;

	// 最大闪烁距离
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlinkCastRange = 3000.f;

	// 目标选择完成回调
	UFUNCTION()
	void GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 目标选择取消回调
	UFUNCTION()
	void GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 传送事件回调（由动画事件触发）
	UFUNCTION()
	void Teleport(FGameplayEventData Payload);

	// 缓存的目标位置数据
	FGameplayAbilityTargetDataHandle BlinkTargetDataHandle;
};
