// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_BlackHole.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UNiagaraSystem;
/**
 * 黑洞目标选择器
 * 用于黑洞技能的范围检测、目标吸引、特效管理等
 */
UCLASS()
class CRUNCH_API ATargetActor_BlackHole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetActor_BlackHole();
	
	// 配置黑洞参数（范围、吸引速度、伤害频率、持续时间、队伍ID）
	void ConfigureBlackHole(
		float InBlackHoleRange,
		float InPullSpeed,
		float InTargetingInterval,
		float InBlackHoleDuration,
		const FGenericTeamId& InTeamId);

	// 设置队伍 ID
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamId) override;

	// 获取队伍 ID
	virtual FGenericTeamId GetGenericTeamId() const override{return TeamId;}

	// 网络属性同步
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 开始目标检测
	virtual void StartTargeting(class UGameplayAbility* Ability) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 确认目标并继续
	virtual void ConfirmTargetingAndContinue() override;

	// 取消目标检测
	virtual void CancelTargeting() override;

	virtual void BeginDestroy() override;
	
private:
	// 黑洞所属队伍ID
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	// 黑洞吸引速度
	float PullSpeed;

	// 检测间隔（秒）
	UPROPERTY()
	float TargetingInterval;

	// 黑洞持续时间
	float BlackHoleDuration;

	// 黑洞持续定时器
	FTimerHandle BlackHoleDurationTimerHandle;
	
	// 黑洞范围
	UPROPERTY(ReplicatedUsing = OnRep_BlackHoleRange)
	float BlackHoleRange;
	// 黑洞范围变化回调
	UFUNCTION()
	void OnRep_BlackHoleRange();

	// 黑洞特效原点参数名
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FName BlackHoleVFXOriginVariableName = "Origin";

	// 黑洞连接特效（Niagara）
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> BlackHoleLinkVFX;

	// 根组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	// 检测球体组件（黑洞范围）
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> DetectionSphereComponent;

	// 黑洞本体特效组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<UParticleSystemComponent> VFXComponent;

	// 进入黑洞范围回调
	UFUNCTION()
	void ActorInBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// 离开黑洞范围回调
	UFUNCTION()
	void ActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 尝试添加目标
	void TryAddTarget(AActor* OtherTarget);

	// 移除目标
	void RemoveTarget(AActor* OtherTarget);

	// 范围内目标与特效映射
	UPROPERTY()
	TMap<AActor*, UNiagaraComponent*> ActorsInRangeMap;

	// 停止黑洞效果
	UFUNCTION()
	void StopBlackHole();

	// 定时检测句柄
	FTimerHandle PeriodicalTargetingTimerHandle;

	// 执行目标检测并报告结果
	void DoTargetCheckAndReport();
};
