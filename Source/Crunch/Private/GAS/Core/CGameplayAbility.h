// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CGameplayAbilityTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UCGameplayAbility();

	// 技能是否可用
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	// 技能冷却
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration;

protected:
	// 获取瞄准目标（按距离和阵营过滤）
	AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;
	
	// 获取拥有者的动画实例
	UAnimInstance* GetOwnerAnimInstance() const;
	// 毫无用武之地
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = true, bool bIgnoreSelf = true) const;

	/**
	* @brief 向命中结果中的 Actor 应用指定的 GameplayEffect
	*
	* 根据传入的 HitResult 和 Effect 类型创建 EffectSpec，并应用到目标 Actor。
	* 适用于攻击判定、技能释放等需要即时生效效果的场景。
	* 
	* @param HitResult 命中信息，包含目标 Actor 和碰撞数据
	* @param GameplayEffect 要应用的 GameplayEffect 类型
	* @param Level 效果等级，默认为 1，用于控制伤害/增益强度
	*/
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	// 将伤害应用于Actor
	void ApplyDamageToActor(AActor* TargetActor,const FGenericDamageEffectDef& Damage, int Level = 1);

	// 将伤害应用到TargetDataHandle中的所有目标
	void ApplyDamageToTargetDataHandle(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FGenericDamageEffectDef& Damage, int Level = 1);

	// 设置伤害
	void MakeDamage(const FGenericDamageEffectDef& Damage, int Level = 1);
	UFUNCTION()
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }

	// 推动自己（如击退/击飞）
	void PushSelf(const FVector& PushVel);
	// 推动目标
	void PushTarget(AActor* Target, const FVector& PushVel);
	// 推动多个目标
	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel);
	// 推动TargetData中的所有目标
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel);

	// 本地播放Montage动画
	void PlayMontageLocally(UAnimMontage* MontageToPlay);
	// 播放完当前分段后停止Montage
	void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop);
	// 获取拥有者的队伍ID
	FGenericTeamId GetOwnerTeamId() const;
	// 判断目标是否为指定阵营
	bool IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const;
	// 获取拥有者角色指针
	ACharacter* GetOwningAvatarCharacter();

	// 发送本地Gameplay事件
	void SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

// 获取骨骼位置Begin~
	// 客户端中获取骨骼位置
	// UFUNCTION(Client, Reliable)
	// void Client_SetAvatarMeshSocketLocation(FName SocketName);
	FVector GetAvatarMeshSocketLocation(FName SocketName) const;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDrawDebug = false;

	// 缓存的拥有者角色指针
	UPROPERTY()
	TObjectPtr<ACharacter> AvatarCharacter;
};
