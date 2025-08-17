// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "CGameState.generated.h"

/**
 * 玩家选择更新委托
 * 当玩家选择状态发生变化时广播
 * @param NewPlayerSelection 新的玩家选择数组
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated, const TArray<FPlayerSelection>& /*NewPlayerSelection*/);

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

	// /**
	//  * 设置角色选择
	//  * @param SelectingPlayer 正在选择的玩家状态
	//  * @param SelectedDefination 选择的角色定义
	//  */
	// void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPA_CharacterDefination* SelectedDefination);

	/**
	 * 检查槽位是否被占用
	 * @param SlotId 要检查的槽位ID
	 * @return 是否被占用
	 */
	bool IsSlotOccupied(uint8 SlotId) const;

	// /**
	//  * 检查角色是否已被选择
	//  * @param Definiation 要检查的角色定义
	//  * @return 是否已被选择
	//  */
	// bool IsDefiniationSelected(const UPA_CharacterDefination* Definiation) const;
	//
	// /**
	//  * 取消角色选择
	//  * @param DefiniationToDeselect 要取消选择的角色定义
	//  */
	// void SetCharacterDeselected(const UPA_CharacterDefination* DefiniationToDeselect);

	
	/** 玩家选择更新事件（多播委托） */
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;

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

	// /**
	//  * 检查是否可以开始比赛
	//  * @return 是否满足开始比赛条件
	//  */
	// bool CanStartMatch() const;

	/**
	 * 复制属性接口实现
	 * 声明需要网络复制的属性
	 */
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > &OutLifetimeProps) const override;

private:
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
