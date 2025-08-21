// 幻雨喜欢小猫咪


#include "MPlayerState.h"

#include "Character/CCharacter.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Framework/CGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Network/TNetStatics.h"

AMPlayerState::AMPlayerState()
{
	bReplicates = true;          // 开启网络复制
	NetUpdateFrequency = 100.f;  // 网络更新频率
}

void AMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMPlayerState, PlayerSelection);
}

void AMPlayerState::BeginPlay()
{
	Super::BeginPlay();
	// 获取游戏状态引用
	CGameState = Cast<ACGameState>(UGameplayStatics::GetGameState(this));

	// 绑定玩家选择更新事件
	if (CGameState)
	{
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &AMPlayerState::PlayerSelectionUpdated);
	}
}

void AMPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	// 将当前玩家的选择状态复制到新玩家状态
	if (AMPlayerState* NewPlayerState = Cast<AMPlayerState>(PlayerState))
	{
		NewPlayerState->PlayerSelection = PlayerSelection;
	}
}

TSubclassOf<APawn> AMPlayerState::GetSelectedPawnClass() const
{
	// 如果已选择角色，加载对应的角色类
	if (PlayerSelection.GetCharacterDefinition())
	{
		return PlayerSelection.GetCharacterDefinition()->LoadCharacterClass();
	}
	return nullptr;
}

FGenericTeamId AMPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelection.GetPlayerSlot() < UTNetStatics::GetPlayerCountPerTeam() 
		? FGenericTeamId{ 0 }  // 队伍0
		: FGenericTeamId{ 1 }; // 队伍1
}

void AMPlayerState::Server_SetSelectedCharacterDefinition_Implementation(const UPDA_CharacterDefinition* NewDefinition)
{
	// 安全检查
	if (!CGameState || !NewDefinition) return;
	
	// 如果角色已被其他玩家选择则退出
	if (CGameState->IsDefinitionSelected(NewDefinition)) return;
	
	// 如果玩家已有选择，先取消旧选择
	if (PlayerSelection.GetCharacterDefinition())
	{
		CGameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());
	}
	
	// 更新选择并通知游戏状态
	PlayerSelection.SetCharacterDefinition(NewDefinition);
	CGameState->SetCharacterSelected(this, NewDefinition);
}

bool AMPlayerState::Server_SetSelectedCharacterDefinition_Validate(const UPDA_CharacterDefinition* NewDefinition)
{
	return true;
}

void AMPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections)
{
	// 在更新列表中找到当前玩家的选择数据
	for (const FPlayerSelection& NewPlayerSelection : NewPlayerSelections)
	{
		if (NewPlayerSelection.IsForPlayer(this))
		{
			// 更新本地玩家选择状态
			PlayerSelection = NewPlayerSelection;
		}
	}
}
