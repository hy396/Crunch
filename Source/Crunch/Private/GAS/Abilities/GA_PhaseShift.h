// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_PhaseShift.generated.h"

class AArcaneTrapActor;

/**
 * 相位转移技能（法师 E）
 * 短暂进入无敌状态并向后位移，原地留下一个定身陷阱
 * 敌人踩到陷阱会被定身并受到魔法伤害
 */
UCLASS()
class CRUNCH_API UGA_PhaseShift : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_PhaseShift();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	// 相位转移动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> PhaseShiftMontage;

	// 后退推力速度
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float BackwardPushSpeed = 1500.f;

	// 陷阱Actor类
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	TSubclassOf<AArcaneTrapActor> TrapActorClass;

	// 陷阱检测半径
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	float TrapRadius = 200.f;

	// 陷阱存在时间
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	float TrapDuration = 4.f;

	// 陷阱激活延迟
	UPROPERTY(EditDefaultsOnly, Category = "Trap")
	float TrapArmDelay = 0.5f;

	// 陷阱伤害效果（魔法伤害）
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> TrapDamageEffect;

	// 定身效果
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> RootEffect;

	// 相位转移视觉标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag PhaseShiftCueTag;

	// 缓存的原地位置（用于放置陷阱）
	FVector CachedOriginLocation;

	// 位移动画事件回调
	UFUNCTION()
	void OnShift(FGameplayEventData Payload);
};
