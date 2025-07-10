// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Character/CCharacter.h"
#include "Minion.generated.h"

/**
 * 小兵AI角色类，继承自ACCharacter
 * 负责小兵的队伍分配、激活状态、目标设置、皮肤切换等功能
 */
UCLASS()
class CRUNCH_API AMinion : public ACCharacter
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
private:
	// 根据队伍ID切换小兵皮肤
	void PickSkinBasedOnTeamID();

	// 队伍ID同步时回调（用于网络同步后自动切换皮肤等）
	virtual void OnRep_TeamID() override;

	// 队伍ID到对应皮肤的映射表
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, TObjectPtr<USkeletalMesh>> SkinMap;
};
