// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "MenuPlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API ALobbyPlayerController : public AMenuPlayerController
{
	GENERATED_BODY()
public:
	/**
	 * 服务器端处理槽位选择变更请求
	 * @param NewSlotID 新的槽位ID
	 * 
	 * 网络调用：客户端请求变更槽位选择时调用
	 * 服务器验证后更新玩家槽位
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSlotSelectionChange(uint8 NewSlotID);
};
