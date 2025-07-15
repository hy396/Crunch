// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Components/DecalComponent.h"
#include "TargetActor_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	ATargetActor_GroundPick();

	// 设置目标区域半径
	void SetTargetAreaRadius(float NewRadius);
	
	// 设置目标检测最大距离
	FORCEINLINE void SetTargetTraceRange(float NewRange) { TargetTraceRange = NewRange; }

	// 确认目标
	virtual void ConfirmTargetingAndContinue() override;

	// 设置目标筛选选项
	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy = true);

	// 设置是否绘制调试信息
	FORCEINLINE void SetShouldDrawDebug(bool bDrawDebug) { bShouldDrawDebug = bDrawDebug; }
private:
	// 贴花（用于显示技能范围）
	UPROPERTY(VisibleDefaultsOnly, Category = "Visual")
	TObjectPtr<UDecalComponent> DecalComp;
	
	// 是否可选敌方
	bool bShouldTargetEnemy = true;

	// 是否可选友方
	bool bShouldTargetFriendly = false;
	
	virtual void Tick(float DeltaTime) override;

	// 获取当前目标点（玩家视角射线检测地面）
	FVector GetTargetPoint() const;

	// 目标区域半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;

	// 目标检测最大距离
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	// 是否绘制调试信息
	bool bShouldDrawDebug = true;
};
