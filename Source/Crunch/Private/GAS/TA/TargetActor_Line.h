// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TargetActor_Line.generated.h"

class UNiagaraComponent;
/**
 * 线性目标检测器
 * 用于技能/能力的线性范围目标检测与反馈
 */
UCLASS()
class CRUNCH_API ATargetActor_Line : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATargetActor_Line();
	/**
	 * 配置目标检测参数
	 * @param NewTargetRange 检测距离
	 * @param NewDetectionCylinderRadius 检测半径
	 * @param NewTargetingInterval 检测频率 /s
	 * @param OwnerTeamId 拥有者队伍ID
	 * @param bShouldDrawDebug 是否绘制调试信息
	 */
	void ConfigureTargetSetting(
		float NewTargetRange,
		float NewDetectionCylinderRadius,
		float NewTargetingInterval,
		FGenericTeamId OwnerTeamId,
		bool bShouldDrawDebug
	);

	// 设置队伍ID
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	// 获取队伍ID
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	// 网络属性同步
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 开始目标检测
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	// 每帧调用，处理目标检测和特效更新
	virtual void Tick(float DeltaTime) override;

	// 销毁时回调
	virtual void BeginDestroy() override;
private:
	// 检测射程
	UPROPERTY(Replicated)
	float TargetRange;

	// 检测圆柱体半径
	UPROPERTY(Replicated)
	float DetectionCylinderRadius;

	// 检测间隔（秒）
	UPROPERTY()
	float TargetingInterval;

	// 所属队伍ID
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	// 是否绘制调试信息
	UPROPERTY()
	bool bDrawDebug;

	// 技能拥有者Actor
	UPROPERTY(Replicated)
	const AActor* AvatarActor;

	// 激光特效长度参数名
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FName LaserVFXLengthParamName = "Length";
	// 根组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> RootComp;

	// TODO: 这个特效换一个东西
	// 激光特效组件
	// UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	// TObjectPtr<UNiagaraComponent> LaserVFX;

	// 激光特效组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<UParticleSystemComponent> LaserFX;
	
	// 激光特效长度参数名
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	FName LaserFXLengthParamName = "length";

	// 激光特效 beamEnd 参数名
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	FName LaserFXBeamEndParamName = "beamEnd";

	// 激光特效 tileVcorrect 参数名
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	FName LaserFXtileVcorrectParamName = "tileVcorrect";

	// 目标检测球体组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	TObjectPtr<USphereComponent> TargetEndDetectionSphere;

	// 定时检测句柄
	FTimerHandle PeriodicalTargetingTimerHandle;

	// 执行目标检测并报告结果
	void DoTargetCheckAndReport();

	// 更新激光特效和检测逻辑
	void UpdateTargetTrace();

	// 判断Actor是否应被报告为目标
	bool ShouldReportActorAsTarget(const AActor* ActorToCheck) const;
};
