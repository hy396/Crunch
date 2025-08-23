// 幻雨喜欢小猫咪


#include "Player/PlayerInfoTypes.h"

#include "GameFramework/PlayerState.h"
#include "Network/TNetStatics.h"

FPlayerSelection::FPlayerSelection()
		:Slot{ GetInvalidSlot() },
		PlayerUniqueId{ FUniqueNetIdRepl::Invalid() },
		PlayerNickName{""}
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState)
			:Slot{ InSlot }
{
	if (InPlayerState)
	{
		// 获取网络唯一ID和名称
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}

bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
	if (!PlayerState)
		return false;
	// 编辑器启动的时候用玩家名字判断
#if WITH_EDITOR
	return PlayerState->GetPlayerName() == PlayerNickName;
#else
	// 运行时用网络唯一ID判断
	return PlayerState->GetUniqueId() == GetPLayerUniqueId();
#endif
}

bool FPlayerSelection::IsValid() const
{
#if WITH_EDITOR
	return true; // 编辑器模式始终返回有效（用于测试）
#else
	// 检查网络唯一ID是否有效
	if (!PlayerUniqueId.IsValid())
		return false;
	// 槽位是否有效
	if (Slot == GetInvalidSlot())
		return false;

	// 检查槽位是否超出双队列限制
	if (Slot >= UTNetStatics::GetPlayerCountPerTeam() * 2)
		return false;

	return true;
#endif
}

uint8 FPlayerSelection::GetInvalidSlot()
{
	// 无效槽位
	return 255;
}
