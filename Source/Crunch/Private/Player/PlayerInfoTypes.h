// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.generated.h"

class APlayerState;

/**
 * 玩家选择信息结构体
 * 存储玩家在游戏大厅中的选择状态，包括槽位、角色定义等信息
 */
USTRUCT()
struct FPlayerSelection
{
	GENERATED_BODY()
public:
	/**
	 * 默认构造函数
	 * 初始化空的玩家选择
	 */
	FPlayerSelection();
	
	/**
	 * 带参数的构造函数
	 * @param InSlot 玩家槽位ID
	 * @param InPlayerState 玩家状态对象
	 */
	FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState);

	/**
	 * 设置玩家槽位
	 * @param NewSlot 新槽位ID
	 */
	FORCEINLINE void SetSlot(uint8 NewSlot) { Slot = NewSlot; }
	
	/**
	 * 获取玩家槽位ID
	 * @return 当前玩家槽位
	 */
	FORCEINLINE uint8 GetPlayerSlot() const { return Slot; }
	
	/**
	 * 获取玩家唯一ID
	 * @return 网络唯一ID副本
	 */
	FORCEINLINE FUniqueNetIdRepl GetPLayerUniqueId() const { return PlayerUniqueId; }
	
	/**
	 * 获取玩家昵称
	 * @return 玩家显示名称
	 */
	FORCEINLINE FString GetPlayerNickName() const { return PlayerNickName; }
	
	/**
	 * 检查是否属于指定玩家
	 * @param PlayerState 待比较的玩家状态对象
	 * @return 是否匹配
	 */
	bool IsForPlayer(const APlayerState* PlayerState) const;
	
	/**
	 * 验证选择数据有效性
	 * @return 是否为有效选择
	 */
	bool IsValid() const;

	/**
	 * 获取无效槽位标识符
	 * @return 无效槽位的数值表示
	 */
	static uint8 GetInvalidSlot();

private:
	/** 玩家在队伍中的槽位ID */
	UPROPERTY()
	uint8 Slot;

	/** 玩家的网络唯一ID */
	UPROPERTY()
	FUniqueNetIdRepl PlayerUniqueId;

	/** 玩家显示名称 */
	UPROPERTY() 
	FString PlayerNickName;
};
