// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "GAS/TA/TargetActor_GroundPick.h"
#include "GA_GroundBlast.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_GroundBlast : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_GroundBlast();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	// 冲击特效Cue标签
	UPROPERTY(EditDefaultsOnly, Category = "Cue")
	FGameplayTag BlastGameplayCueTag;
	
	// 技能目标区域半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGenericDamageEffectDef DamageEffectDef;
	// 目标检测最大距离
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;
	
	// 地面选点目标Actor类
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;
	
	// 瞄准动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> TargetingMontage;

	// 释放动画
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> CastMontage;
	
	// 目标确认回调
	UFUNCTION()
	void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	// 目标取消回调
	UFUNCTION()
	void TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);


	int32 NumCount = 3;
};
