// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "BurningGroundActor.generated.h"

class USphereComponent;

/**
 * 燃烧地面Actor
 * 持续一段时间的区域DOT，周期性对范围内敌人施加灼烧伤害
 * 由GA_MeteorStrike在陨石落点生成
 */
UCLASS()
class CRUNCH_API ABurningGroundActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ABurningGroundActor();

	/**
	 * 初始化燃烧区域
	 * @param InTeamId 所属队伍
	 * @param InBurnSpec 灼烧伤害效果
	 * @param InRadius 区域半径
	 * @param InDuration 持续时间
	 * @param InTickInterval 伤害间隔
	 */
	void InitializeBurningGround(
		FGenericTeamId InTeamId,
		FGameplayEffectSpecHandle InBurnSpec,
		float InRadius,
		float InDuration,
		float InTickInterval
	);

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> DamageSphere;

	FGenericTeamId TeamId;
	FGameplayEffectSpecHandle BurnSpecHandle;

	FTimerHandle TickTimerHandle;
	FTimerHandle DurationTimerHandle;

	// 周期性伤害
	void DoBurnTick();

	// 区域超时自毁
	void OnDurationExpired();
};
