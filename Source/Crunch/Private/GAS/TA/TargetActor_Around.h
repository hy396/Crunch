// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Around.generated.h"

class USphereComponent;
/**
 * 圆形范围目标检测器，用于检测角色周围的敌对目标
 * 实现队伍关系接口(IGenericTeamAgentInterface)用于敌我识别
 */
UCLASS()
class ATargetActor_Around : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ATargetActor_Around();
	
	// 配置检测参数：检测半径、队伍ID和本地视觉提示标签
	void ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId, const FGameplayTag& InLocalGameplayCueTag);

	/** 实现IGenericTeamAgentInterface接口 - 设置队伍ID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	/** 实现IGenericTeamAgentInterface接口 - 获取队伍ID */
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	
	/** 网络属性复制 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId; // 当前检测器的队伍ID（用于敌我识别）

	// 根组件（用于位置定位）
	UPROPERTY(VisibleDefaultsOnly, Category = "Comp")
	TObjectPtr<USceneComponent> RootComp;

	// 球形碰撞体（用于范围检测）
	UPROPERTY(VisibleDefaultsOnly, Category = "Targeting")
	TObjectPtr<USphereComponent> DetectionSphere;

	// 检测半径（网络同步）
	UPROPERTY(ReplicatedUsing = OnRep_TargetDetectionRadiusReplicated)
	float TargetDetectionRadius;

	// 检测半径复制回调（客户端同步时更新碰撞体大小）
	UFUNCTION()
	void OnRep_TargetDetectionRadiusReplicated();

	// 本地视觉提示标签（命中目标时播放的视觉效果）
	UPROPERTY(Replicated)
	FGameplayTag LocalGameplayCueTag;

	// 碰撞体进入检测范围时的回调
	UFUNCTION()
	void ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
