// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "UpperCut.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UUpperCut : public UCGameplayAbility
{
	GENERATED_BODY()
public:	
	UUpperCut();

	// 激活技能时调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	// 上勾拳击飞阶段的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;
	
	// 上勾拳击飞速度
	UPROPERTY(EditDefaultsOnly, Category = "Launch", meta = (DisplayName = "击飞力的大小"))
	float UpperCutLaunchSpeed = 1000.f;
	
	// 上勾拳动画Montage
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> UpperCutMontage;

	// 启动击飞效果
	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

};
