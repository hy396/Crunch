// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Actor/ProjectileActor.h"
#include "GAS/Core/CGameplayAbility.h"
#include "GA_SwordAura.generated.h"


USTRUCT()
struct FSwordSocketPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName StartSocket;

	UPROPERTY(EditAnywhere, Category = "Sockets")
	FName EndSocket;

	FSwordSocketPair()
		: StartSocket(NAME_None)
		, EndSocket(NAME_None)
	{
	}

	FSwordSocketPair(FName InStartSocket, FName InEndSocket)
		: StartSocket(InStartSocket)
		, EndSocket(InEndSocket)
	{
	}
};
/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_SwordAura : public UCGameplayAbility
{
	GENERATED_BODY()
public:

	UGA_SwordAura();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 挥砍动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> SwingMontage;

	// 剑气投射物类
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AProjectileActor> SwordAuraProjectileClass;

	// 剑气伤害效果
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGenericDamageEffectDef SwordAuraDamageEffect;

	// 剑气速度
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float SwordAuraSpeed = 2000.0f;

	// 剑气最大飞行距离
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float SwordAuraMaxDistance = 3000.0f;

	// // 起始骨骼名称
	// UPROPERTY(EditDefaultsOnly, Category = "Skeleton")
	// FName StartSocketName = "sword_start";
	//
	// // 结束骨骼名称
	// UPROPERTY(EditDefaultsOnly, Category = "Skeleton")
	// FName EndSocketName = "sword_end";

	// 剑气的数量
	UPROPERTY(EditDefaultsOnly, Category = "SwordAura")
	int32 SwordAuraCount = 12;

	// 剑气骨骼位置
	UPROPERTY(EditDefaultsOnly, Category = "Skeleton")
	TArray<FSwordSocketPair> SocketNames;

private:
	// 已经发射的剑气数量
	int32 SwordAuraCountSpawned = 0;
	// 当前使用的骨骼位置索引
    int32 SocketPositionIndex = 0;
	// 发射剑气
	UFUNCTION()
	void SpawnSwordAura(FGameplayEventData Payload);
	
	// 计算剑气发射位置和方向
	void CalculateSwordAuraSpawnParams(FVector& OutSpawnLocation, FRotator& OutSpawnRotation);
};
