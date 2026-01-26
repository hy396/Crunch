// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AISenseConfig_Sight.h"
#include "CAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()
public:

	// ACAIController();
	// Sets default values for this actor's properties
	// 设置此角色属性的默认值
	ACAIController(const FObjectInitializer& ObjectInitializer);
	// 当AI控制器获取到Pawn时调用
	virtual void OnPossess(APawn* InPawn) override;

	// 获取团队态度
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	// 设置团队ID
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	// 设置视觉感知参数
	void SetSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees);

	virtual void BeginPlay() override;
private:
	// 黑板中用于存储目标的Key名
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetBlackboardKeyName = "Target";

	// 行为树
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	// AI感知组件（用于感知敌人等）
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	
	// 视觉感知配置（用于设置视野范围、角度等）
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// 感知到目标时的回调
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

	// 感知目标丢失时的回调
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);

	// 获取当前黑板中的目标对象
	const UObject* GetCurrentTarget() const;
	
	// 设置当前目标到黑板
	void SetCurrentTarget(AActor* NewTarget);

	// 获取下一个感知到的敌人（用于目标切换）
	AActor* GetNextPerceivedActor() const;

	// 如果目标已死亡，则忘记该目标（清理感知数据）
	void ForgetActorIfDead(AActor* ActorToForget);

	// 清除并禁用所有感知（如死亡时调用）
	void ClearAndDisableAllSenses();

	// 启用所有感知（如复活时调用）
	void EnableAllSenses();

	// 监听Pawn死亡Tag变化，控制AI逻辑启停
	void PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count);
	// 监听Pawn眩晕Tag变化，控制AI逻辑启停
	void PawnStunTagUpdated(const FGameplayTag Tag, int32 Count);

	// 标记AI当前是否处于死亡状态
	bool bIsPawnDead = false;

	// 是否启用人群避让
	UPROPERTY(EditDefaultsOnly, Category="Detor Crowd Avoidance Config")
	bool bEnableCrowdAvoidance = true;	

	// meta = (EditCondition = "bEnableCrowdAvoidance")) 意味着只有当 bEnableCrowdAvoidance 为 true 时，才会显示这个属性（即只有在启用人群避让时，才会显示这个属性，才能进行该项设置）
	// , UIMin = "1", UIMax = "4" 则是设置了该属性的最小值和最大值，让我能在编辑器中可以像滑动条一样调整这个整数值，进而控制人群避让的质量
	UPROPERTY(EditDefaultsOnly, Category="Detor Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;	// 人群避让质量

	// 人群避让范围
	UPROPERTY(EditDefaultsOnly, Category="Detor Crowd Avoidance Config", meta = (EditCondition = "bEnableCrowdAvoidance"))
	float CollisionQueryRange = 600.0f;		
};
