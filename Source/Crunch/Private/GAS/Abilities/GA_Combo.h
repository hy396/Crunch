// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Combo : public UCGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_Combo();

	// 技能激活
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	/**
	 * @brief 设置监听连击输入按下的异步任务
	 *
	 * 该函数用于创建并初始化一个等待玩家按下连击输入的任务，
	 * 通常在连击能力激活时调用。
	 */
	void SetupWaitComboInputPress();

	/**
	 * @brief 处理连击输入按下的回调函数
	 *
	 * 当玩家按下连击输入时触发此函数，用于执行连击逻辑，
	 * 如切换到下一个连击阶段或播放下一段动画。
	 *
	 * @param TimeWaited 输入按下前的等待时间（秒）
	 */
	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	/**
	 * @brief 尝试提交当前连击动作
	 *
	 * 该函数用于检查是否满足连击触发条件，并尝试激活下一个连击阶段。
	 * 若条件不满足，则可能重置连击状态或忽略此次输入。
	 */
	void TryCommitCombo();

	// 默认伤害效果类
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;
	
	// 蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ComboMontage;

	// 获取当前连击的伤害效果
	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;
	
	// Tag传入事件绑定
	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData Data);

	UFUNCTION()
	void DoDamage(FGameplayEventData Data);
	
	FName NextComboName;

	FName JumpComboName;
};
