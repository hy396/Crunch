// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendTargetGroup.generated.h"

/**
 * 
 */
UCLASS()
class UAN_SendTargetGroup : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	// 触发的GameplayCue标签（如命中特效、音效等）
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTagContainer TriggerGameplayCueTags;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "目标阵营"))
	TEnumAsByte<ETeamAttitude::Type> TargetTeam = ETeamAttitude::Hostile;

	// 球形检测半径
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "球体半径"))
	float SphereSweepRadius = 60.f;

	// 是否绘制调试轨迹
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "绘制调试轨迹"))
	bool bDrawDebug = false;
	
	// 是否忽略自己
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability", meta = (DisplayName = "忽略自身"))
	bool bIgnoreOwner = true;
	
	// 发送的Tag
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTag EventTag;
	
	// 骨骼名称
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	TArray<FName> TargetSocketNames;

	// 触发本地GameplayCue（如播放命中特效、音效等）
	void SendLocalGameplayCue(const FHitResult& HitResult) const;
};
