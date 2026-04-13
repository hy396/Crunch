// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Actor/ProjectileActor.h"
#include "ArcaneOrbProjectile.generated.h"

/**
 * 奥术弹投射物 —— 穿透型投射物
 * 不在命中时销毁，穿透所有敌人
 * 追踪飞行距离，用于距离增伤计算
 * 每个目标只命中一次
 */
UCLASS()
class CRUNCH_API AArcaneOrbProjectile : public AProjectileActor
{
	GENERATED_BODY()
public:
	AArcaneOrbProjectile();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void Tick(float DeltaTime) override;

	/** 获取已飞行距离占最大距离的比例（0~1） */
	float GetDistanceRatio() const;

	/** 设置最大飞行距离（用于距离比例计算） */
	void SetMaxDistance(float InMaxDistance) { MaxDistance = InMaxDistance; }

	/** 设置法力回复GE（命中时回复施法者法力） */
	void SetManaRestoreEffect(FGameplayEffectSpecHandle InEffect) { ManaRestoreEffectHandle = InEffect; }

	/** 设置距离伤害倍率（最远处的伤害倍率） */
	void SetDistanceDamageMultiplier(float InMultiplier) { DistanceDamageMultiplier = InMultiplier; }

private:
	// 已命中的目标列表（防止同一目标被多次命中）
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AlreadyHitActors;

	// 起始位置
	FVector StartLocation;

	// 最大飞行距离
	float MaxDistance = 3500.f;

	// 已飞行距离
	float TraveledDistance = 0.f;

	// 距离伤害倍率
	float DistanceDamageMultiplier = 1.5f;

	// 法力回复效果句柄
	FGameplayEffectSpecHandle ManaRestoreEffectHandle;
};
