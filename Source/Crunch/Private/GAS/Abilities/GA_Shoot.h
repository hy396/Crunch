// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_Shoot.generated.h"

class AProjectileActor;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Shoot : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	// 构造函数
	UGA_Shoot();

	// 激活能力（开始射击）
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 输入释放（停止射击）
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// 结束能力（能力生命周期结束）
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// 网络属性同步
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	// 命中弹道时应用的效果
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	FGenericDamageEffectDef ProjectileHitEffect;

	// 子弹速度
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float ShootProjectileSpeed = 2000.f;

	// 子弹射程
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float ShootProjectileRange = 3000.f;

	// 子弹Actor类
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	TSubclassOf<AProjectileActor> ProjectileClass;
	
	// 射击动画
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	TObjectPtr<UAnimMontage> ShootMontage;

	// 开始射击事件处理
	UFUNCTION()
	void StartShooting(FGameplayEventData Payload);

	// 停止射击事件处理
	UFUNCTION()
	void StopShooting(FGameplayEventData Payload);

	// 发射子弹
	UFUNCTION()
	void ShootProjectile(FGameplayEventData Payload);

	// 获取有效瞄准目标
	AActor* GetAimTargetIfValid() const;

	// 当前瞄准目标
	UPROPERTY()
	TObjectPtr<AActor> AimTarget;

	// 目标的能力系统组件
	UPROPERTY()
	UAbilitySystemComponent* AimTargetAbilitySystemComponent;

	// 检查瞄准目标的定时器句柄
	FTimerHandle AimTargetCheckTimerHandle;

	// 查找瞄准目标
	void FindAimTarget();

	// 检查瞄准目标的时间间隔
	UPROPERTY(EditDefaultsOnly, Category = "Target")
	float AimTargetCheckTimeInterval = 0.1f;

	// 启动瞄准目标检测定时器
	void StartAimTargetCheckTimer();

	// 停止瞄准目标检测定时器
	void StopAimTargetCheckTimer();

	// 是否有有效目标
	bool HasValidTarget() const;

	// 目标是否在射程内
	bool IsTargetInRange() const;

	// 目标死亡标签更新回调
	void TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount);

	// // 服务器设置弹道位置
	// UFUNCTION(Server, Reliable)
	// void ServerSetProjectileSpawnLocation(const FVector& Location);
	// // 存储客户端传递的位置
	// UPROPERTY(Replicated)
	// FVector ServerSocketLocation;
	// UPROPERTY(Replicated)
	// bool bHasClientLocation = false; // 标记已收到客户端数据
};
