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

	ACAIController();

	virtual void OnPossess(APawn* InPawn) override;
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

};
