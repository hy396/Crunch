// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "MPlayerState.generated.h"

class ACGameState;
class UPDA_CharacterDefinition;
/**
 * 自定义玩家状态类，存储玩家在游戏中的个性化状态数据
 * 包括角色选择、队伍分配等信息（网络同步）
 */
UCLASS()
class CRUNCH_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	AMPlayerState();
	// 网络复制属性
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	// 玩家状态复制
	virtual void CopyProperties(APlayerState* PlayerState) override;

	// 获取玩家选择的角色Pawn类
	TSubclassOf<APawn> GetSelectedPawnClass() const;
	
	// 根据玩家槽位分配队伍ID (0队或1队)
	FGenericTeamId GetTeamIdBasedOnSlot() const;

	// 服务器RPC：设置玩家选择的角色定义
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacterDefinition(const UPDA_CharacterDefinition* NewDefinition);

private:	
	// 玩家选择信息（角色、槽位等），网络同步
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;

	// 指向游戏状态的引用
	UPROPERTY()	
	TObjectPtr<ACGameState> CGameState;

	// 当游戏状态中的玩家选择更新时调用
	void PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections);
};
