// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "ArcaneTrapActor.generated.h"

class USphereComponent;

/**
 * 奥术陷阱Actor
 * 放置在地面，延迟后激活，敌人踩到后触发定身+伤害，随后自毁
 * 由GA_PhaseShift生成
 */
UCLASS()
class CRUNCH_API AArcaneTrapActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AArcaneTrapActor();

	/**
	 * 初始化陷阱
	 * @param InTeamId 所属队伍ID
	 * @param InDamageSpec 触发时的伤害效果
	 * @param InRootSpec 触发时的定身效果
	 * @param InRadius 检测半径
	 * @param InArmDelay 激活延迟（秒）
	 * @param InDuration 陷阱存在时间（秒）
	 */
	void InitializeTrap(
		FGenericTeamId InTeamId,
		FGameplayEffectSpecHandle InDamageSpec,
		FGameplayEffectSpecHandle InRootSpec,
		float InRadius,
		float InArmDelay,
		float InDuration
	);

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	// 根组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	// 检测球体
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> DetectionSphere;

	// 陷阱视觉提示标签
	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag TrapArmedCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag TrapTriggeredCueTag;

	FGenericTeamId TeamId;
	FGameplayEffectSpecHandle DamageSpecHandle;
	FGameplayEffectSpecHandle RootSpecHandle;
	bool bArmed = false;

	FTimerHandle ArmTimerHandle;
	FTimerHandle DurationTimerHandle;

	// 延迟后激活陷阱
	void ArmTrap();

	// 陷阱超时自毁
	void OnDurationExpired();

	// 敌人进入检测范围
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
