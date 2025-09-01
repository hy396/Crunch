// 幻雨喜欢小猫咪


#include "LobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Framework/CGameState.h"
#include "Framework/MGameInstance.h"
#include "UI/Lobby/LobbyWidget.h"
#include "UI/Gameplay/Chat/ChatWidget.h"
#include "Engine/World.h"
#include "GenericTeamAgentInterface.h"
#include "MPlayerState.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	// 设置bAutoManageActiveCameraTarget为false,表示不自动管理摄像机目标（由大厅逻辑手动控制）
	bAutoManageActiveCameraTarget = false;
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// 初始化UI组件
	GetLobbyWidget();
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// 大厅阶段不需要特殊的输入处理，聊天框一直显示可直接输入
}

void ALobbyPlayerController::Server_RequestSlotSelectionChange_Implementation(uint8 NewSlotID)
{
	if (!GetWorld())
		return;

	// 获取当前游戏状态
	ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
	if (!CGameState)
		return;

	// 委托给游戏状态管理器处理槽位变更
	CGameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotID);
} 

bool ALobbyPlayerController::Server_RequestSlotSelectionChange_Validate(uint8 NewSlotID)
{
	return true;
}

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;

	// 遍历所有玩家控制器
	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		// 
		ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(*PlayerControllerIterator);
		if (PlayerController)
		{
			PlayerController->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	// 获取当前游戏实例
	if (UMGameInstance* CGameInstance = GetGameInstance<UMGameInstance>())
	{
		// 启动比赛流程
		CGameInstance->StartMatch();
	}
}

bool ALobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	// 触发界面切换委托
	OnSwitchToHeroSelection.ExecuteIfBound();
}

void ALobbyPlayerController::Server_SendChatMessage_Implementation(const FString& Message, EChatChannelType ChannelType)
{
	if (!HasAuthority() || !GetWorld()) return;

	// 获取发送者信息
	AMPlayerState* SenderPlayerState = GetPlayerState<AMPlayerState>();
	if (!SenderPlayerState) return;

	FString SenderName = SenderPlayerState->GetPlayerName();
	// 大厅阶段使用基于槽位的团队ID判断
	FGenericTeamId SenderTeamId = SenderPlayerState->GetTeamIdBasedOnSlot();

	// 大厅只支持队伍聊天，直接设置为Team频道
	FChatMessage ChatMessage(SenderName, Message, EChatChannelType::Team, SenderTeamId);

	UE_LOG(LogTemp, Warning, TEXT("大厅聊天 - 服务器收到消息：%s 来自：%s (队伍ID:%d)"), *Message, *SenderName, SenderTeamId.GetId());

	// 只向同队队员广播消息
	for (FConstPlayerControllerIterator PlayerControllerIterator = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIterator; ++PlayerControllerIterator)
	{
		ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(*PlayerControllerIterator);
		if (PlayerController)
		{
			// 使用基于槽位的团队ID进行同队判断
			AMPlayerState* ReceiverPlayerState = PlayerController->GetPlayerState<AMPlayerState>();
			if (ReceiverPlayerState)
			{
				FGenericTeamId ReceiverTeamId = ReceiverPlayerState->GetTeamIdBasedOnSlot();
				
				// 只有同队队员才能收到消息
				if (SenderTeamId == ReceiverTeamId)
				{
					PlayerController->Client_ReceiveChatMessage(ChatMessage);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("大厅聊天 - 玩家 %s (队伍ID:%d) 不在同一队伍，不接收消息"), *ReceiverPlayerState->GetPlayerName(), ReceiverTeamId.GetId());
				}
			}
		}
	}
}

bool ALobbyPlayerController::Server_SendChatMessage_Validate(const FString& Message, EChatChannelType ChannelType)
{
	// 基本验证：消息不能为空且长度合理
	return !Message.IsEmpty() && Message.Len() <= 200;
}

void ALobbyPlayerController::Client_ReceiveChatMessage_Implementation(const FChatMessage& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("大厅聊天 - 客户端收到消息：%s 来自：%s"), *Message.MessageContent, *Message.SenderName);

	// 获取当前的大厅UI
	if (ULobbyWidget* LobbyWidget = GetLobbyWidget())
	{
		// 判断发送者类型
		bool bIsSelf = false;
		bool bIsTeammate = false;
		
		// 判断是否是自己发送的消息
		AMPlayerState* MyPlayerState = GetPlayerState<AMPlayerState>();
		if (MyPlayerState)
		{
			FString MyPlayerName = MyPlayerState->GetPlayerName();
			bIsSelf = (MyPlayerName == Message.SenderName);
			
			// 如果不是自己，使用基于槽位的团队ID判断是否是队友
			if (!bIsSelf)
			{
				// 获取自己的团队ID(基于槽位)
				FGenericTeamId MyTeamId = MyPlayerState->GetTeamIdBasedOnSlot();
				bIsTeammate = (MyTeamId == Message.SenderTeamId);
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("大厅聊天 - 发送者类型判断：自己=%s 队友=%s"), 
		       bIsSelf ? TEXT("true") : TEXT("false"), 
		       bIsTeammate ? TEXT("true") : TEXT("false"));
		
		// 使用统一的AddMessageToChat方法
		if (UChatWidget* ChatWidget = LobbyWidget->GetChatWidget())
		{
			ChatWidget->AddMessageToChat(Message, bIsSelf, bIsTeammate);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("无法获取ChatWidget，无法显示聊天消息"));
		}
		
		// 显示弹幕消息（大厅弹幕效果）
		LobbyWidget->ShowBarrageMessage(Message, bIsSelf, bIsTeammate);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("无法获取LobbyWidget，无法显示聊天消息"));
	}
}

ULobbyWidget* ALobbyPlayerController::GetLobbyWidget()
{
	// 如果已经缓存了且对象有效，直接返回
	if (CachedLobbyWidget && IsValid(CachedLobbyWidget))
	{
		return CachedLobbyWidget;
	}
	
	// 通过MenuWidget获取并缓存LobbyWidget
	if (UUserWidget* CacheMenuWidget = GetMenuWidget())
	{
		if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(CacheMenuWidget))
		{
			// 缓存结果以减少后续的Cast操作
			CachedLobbyWidget = LobbyWidget;
			return LobbyWidget;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("无法获取LobbyWidget，请检查MenuWidget是否正确设置"));
	return nullptr;
}

// IChatInterface 接口实现
void ALobbyPlayerController::SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType)
{
	// 调用实际的RPC函数
	Server_SendChatMessage(Message, ChannelType);
}

void ALobbyPlayerController::ReceiveChatMessageFromServer(const FChatMessage& Message)
{
	// 调用实际的RPC函数
	Client_ReceiveChatMessage(Message);
}


