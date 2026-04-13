// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Actor/ProjectileActor.h"
#include "BoomerangAxeProjectile.generated.h"

/**
 * 回旋斧投射物
 * 飞到最大距离后自动反向飞回Owner
 * 去程和回程均可造成伤害
 * 返回到Owner身边时触发GameplayEvent通知技能
 */
UCLASS()
class CRUNCH_API ABoomerangAxeProjectile : public AProjectileActor
{
	GENERATED_BODY()
public:
	ABoomerangAxeProjectile();

	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** 设置回旋参数 */
	void SetReturnEventTag(const FGameplayTag& InTag) { ReturnEventTag = InTag; }

private:
	// 是否正在回旋返回
	bool bIsReturning = false;

	// 返回时通知Owner的事件Tag
	FGameplayTag ReturnEventTag;

	// 去程已命中目标（防重复）
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> OutwardHitActors;

	// 回程已命中目标（防重复）
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ReturnHitActors;

	// 回旋返回的捕获距离（靠近Owner这么近就算返回成功）
	UPROPERTY(EditDefaultsOnly, Category = "Boomerang")
	float ReturnCaptureRadius = 200.f;

	// 开始回旋返回
	void StartReturn();

	// 检查是否已命中过（去程/回程分别检查）
	bool HasAlreadyHit(AActor* Actor) const;
	void RecordHit(AActor* Actor);
};
