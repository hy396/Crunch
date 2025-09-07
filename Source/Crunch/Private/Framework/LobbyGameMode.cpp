// 幻雨喜欢小猫咪


#include "LobbyGameMode.h"

#include "Network/TGameSession.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
	// 确保大厅模式也使用自定义的 GameSession 类，以便正确处理玩家连接事件
	GameSessionClass = ATGameSession::StaticClass();
}
