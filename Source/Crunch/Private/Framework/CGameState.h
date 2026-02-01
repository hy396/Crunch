// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/MPlayerState.h"
#include "Player/PlayerInfoTypes.h"
#include "CGameState.generated.h"

class UPDA_CharacterDefinition;
/**
 * 玩家选择更新委托
 * 当玩家选择状态发生变化时广播
 * @param NewPlayerSelection 新的玩家选择数组
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /*NewPlayerSelection*/);

/**
 * 玩家击杀事件委托
 * 当有玩家被击杀时广播
 * @param KillerState 击杀者玩家状态
 * @param VictimState 被击杀者玩家状态
 * @param AssistStates 助攻者玩家状态列表
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPlayerKilled, AMPlayerState* /*KillerState*/, AMPlayerState* /*VictimState*/, const TArray<AMPlayerState*>& /*AssistStates*/);

/**
 * 游戏状态类（CGameState）
 * 管理大厅阶段的玩家选择状态，处理角色选择和队伍槽位分配
 * 负责维护玩家选择数据并提供相关操作接口
 */
UCLASS()
class CRUNCH_API ACGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	/**
	 * 请求更改玩家选择槽位
	 * @param RequestingPlayer 请求更改的玩家状态
	 * @param DesiredSlot 目标槽位ID
	 */
	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);

	/**
	 * 设置角色选择
	 * @param SelectingPlayer 正在选择的玩家状态
	 * @param SelectedDefinition 选择的角色定义
	 */
	void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPDA_CharacterDefinition* SelectedDefinition);

	/**
	 * 检查槽位是否被占用
	 * @param SlotId 要检查的槽位ID
	 * @return 是否被占用
	 */
	bool IsSlotOccupied(uint8 SlotId) const;

	/**
	 * 检查角色是否已被选择
	 * @param Definition 要检查的角色定义
	 * @return 是否已被选择
	 */
	bool IsDefinitionSelected(const UPDA_CharacterDefinition* Definition) const;
	
	/**
	 * 取消角色选择
	 * @param DefinitionToDeselect 要取消选择的角色定义
	 */
	void SetCharacterDeselected(const UPDA_CharacterDefinition* DefinitionToDeselect);

	
	/** 玩家选择更新事件（多播委托） */
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;

	/** 玩家击杀事件（多播委托） */
	FOnPlayerKilled OnPlayerKilled;

	/**
	 * 触发玩家击杀事件（服务器调用，客户端自动同步）
	 * @param KillerState 击杀者玩家状态
	 * @param VictimState 被击杀者玩家状态
	 * @param AssistStates 助攻者玩家状态列表
	 */
	// UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "Game State")
	// void Server_NotifyPlayerKilled(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates);
// 	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "Game State")
	// void Server_NotifyPlayerKilled(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates);
	//
	// /**
	//  * 客户端接收击杀事件
	//  */
	// UFUNCTION(Client, Reliable)
	// void Client_OnPlayerKilled(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates);
	
	/**
	 * 触发玩家击杀事件 (广播给所有人)
	 * @param KillerState 击杀者玩家状态
	 * @param VictimState 被击杀者玩家状态
	 * @param AssistStates 助攻者玩家状态列表
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnPlayerKilled(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates);
	/**
	 * 获取玩家选择数组
	 * @return 当前所有玩家的选择信息数组
	 */
	const TArray<FPlayerSelection>& GetPlayerSelection() const;

	/**
	 * 检查是否可以开始英雄选择
	 * @return 是否满足开始条件
	 */
	bool CanStartHeroSelection() const;

	/**
	 * 检查是否可以开始比赛
	 * @return 是否满足开始比赛条件
	 */
	bool CanStartMatch() const;
	// 添加队伍一玩家击杀数
	void AddTeamOnePlayerKillCount();
	// 添加队伍二玩家击杀数
	void AddTeamTwoPlayerKillCount();
	/**
	 * 复制属性接口实现
	 * 声明需要网络复制的属性
	 */
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

	/**
	* 为新加入的玩家自动分配一个空槽位
	* @param NewPlayerState 新加入的玩家状态
	* @return 是否成功分配槽位
	*/
	UFUNCTION(BlueprintCallable, Category = "Player Selection")
	bool AutoAssignEmptySlot(const APlayerState* NewPlayerState);
	
	/**
	* 移除离开玩家的选择记录
	* @param LeavingPlayerId 离开玩家的唯一ID
	*/
	void RemovePlayerSelection(const FUniqueNetIdRepl& LeavingPlayerId);
		
private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamOnePlayerKillCount)
	int32 TeamOnePlayerKillCount;

	UPROPERTY(ReplicatedUsing = OnRep_TeamTwoPlayerKillCount)
	int32 TeamTwoPlayerKillCount;

	UFUNCTION()
	void OnRep_TeamOnePlayerKillCount();

	UFUNCTION()
	void OnRep_TeamTwoPlayerKillCount();
	/**
	 * 玩家选择数组（网络复制）
	 * 存储所有玩家当前的槽位和角色选择信息
	 */
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;

	/**
	 * 玩家选择数组复制回调
	 * 当网络同步玩家选择数据时触发
	 */
	UFUNCTION()
	void OnRep_PlayerSelectionArray();
};
