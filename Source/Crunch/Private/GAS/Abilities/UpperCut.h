// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
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
	// TODO: 可能在这里添加手动结束任务的逻辑
	// virtual void K2_EndAbility() override;

	// 激活技能时调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	// 连招阶段对应的伤害效果定义表
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<FName, FGenericDamageEffectDef> ComboDamageMap;
	
	// 上勾拳击飞阶段的伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;
	
	// 上勾拳击飞速度
	UPROPERTY(EditDefaultsOnly, Category = "Launch", meta = (DisplayName = "击飞力的大小"))
	float UpperCutLaunchSpeed = 1000.f;

	// 空中连招的力
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpperComboHoldSpeed = 100.f;
	
	// 上勾拳动画Montage
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> UpperCutMontage;

	// 获取当前连招阶段的伤害效果定义
	const FGenericDamageEffectDef* GetDamageEffectDefForCurrentCombo() const;
	
	// 启动击飞效果
	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);
	
	// 处理连招阶段切换事件
	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);

	// 处理连招提交事件
	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData EventData);

	// 处理连招伤害事件
	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData EventData);
	
	// 下一个连招阶段的名称
	FName NextComboName;
};
