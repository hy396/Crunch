// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackWindow.generated.h"

/**
 * 
 */
UCLASS()
class UANS_AttackWindow : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	//virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	// 触发的GameplayCue标签（如命中特效、音效等）
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTagContainer TriggerGameplayCueTags;
	
	// 目标阵营过滤（如只攻击敌人）
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "目标阵营"))
	TEnumAsByte<ETeamAttitude::Type> TargetTeam = ETeamAttitude::Hostile;

	// 球形检测半径
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "球体半径"))
	float SphereSweepRadius = 60.f;

	// 是否绘制调试轨迹
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "绘制调试轨迹"))
	bool bDrawDebug = true;
	
	// 是否忽略自己
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "忽略自身"))
	bool bIgnoreOwner = true;

	// 是否使用武器,万一我想玩刀了
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "使用武器"))
	bool bUseWeapon = false;

	// 发送的Tag
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTag EventTag;

	// 骨骼名称
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	TArray<FName> TargetSocketNames;

	// 触发本地GameplayCue（如播放命中特效、音效等）
	void SendLocalGameplayCue(const FHitResult& HitResult) const;

	/*额外添加*/
	// 记录碰撞点前一帧的位置
	TArray<FVector> Locations;
	// 碰撞到的 Actor
	// TSet<TWeakObjectPtr<AActor>> HitActors;
	TArray<TWeakObjectPtr<AActor>> HitActors;

	// 后续做好的了武器可以用接口传过来，cast也可以
	// 当前攻击的武器, 我不想一直调用武器组件的接口
	mutable TWeakObjectPtr<USkeletalMeshComponent> AttackMesh;

	mutable TWeakInterfacePtr<IGenericTeamAgentInterface> OwnerTeamInterface;

	// 定时器代替帧检测,帧检测会丢帧
	// FTimerHandle UpDataTimerHandle;
	//
	// UPROPERTY(EditDefaultsOnly, Category = "Death", meta = (DisplayName = "检测时间间隔"))
	// float UpDataTime = 0.03f;
	//
	// void UpData(USkeletalMeshComponent* MeshComp, AActor* OwnerActor);

};
