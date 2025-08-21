// 幻雨喜欢小猫咪


#include "LobbyWidget.h"

#include "CharacterDisplay.h"
#include "CharacterEntryWidget.h"
#include "PlayerTeamLayoutWidget.h"
#include "TeamSelectionWidget.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/CAssetManager.h"
#include "Framework/CGameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Network/TNetStatics.h"
#include "Player/LobbyPlayerController.h"
#include "Player/MPlayerState.h"
#include "UI/Gameplay/Abilities/AbilityListView.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateTeamSelectionSlots();
	// 配置游戏状态
	ConfigureGameState();
	// 获取玩家控制器
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	if (LobbyPlayerController)
	{
		// 绑定英雄选择切换界面事件
		LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ULobbyWidget::SwitchToHeroSelection);
	}
	// 绑定开始英雄选择按钮事件
	StartHeroSelectionButton->SetIsEnabled(false);
	StartHeroSelectionButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartHeroSelectionButtonClicked);

	// 绑定开始比赛按钮事件
	StartMatchButton->SetIsEnabled(false);
	StartMatchButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartMatchButtonClicked);

	// 异步加载角色定义数据
	UCAssetManager::Get().LoadCharacterDefinitions(FStreamableDelegate::CreateUObject(this, &ULobbyWidget::CharacterDefinitionLoaded));

	if (CharacterSelectionTileView)
	{
		// 绑定角色选择事件
		CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ULobbyWidget::CharacterSelected);
	}
	
	SpawnCharacterDisplay(); // 生成角色预览Actor
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();

	// TODO: 正常来说是这样操作
	// 生成两队玩家槽
	for (int32 i = 0; i < UTNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		// 创建槽位
		if (UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass))
		{
			// 设置槽ID
			NewSelectionSlot->SetSlotID(i);
			// 添加到网格布局
			if (UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot))
			{
				// 计算行列位置
				int32 Row = i % UTNetStatics::GetPlayerCountPerTeam();
				int32 Column = i < UTNetStatics::GetPlayerCountPerTeam() ? 0 : 1;

				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Column);
			}
			// 绑定槽点击事件产生的广播委托
			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ULobbyWidget::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotID)
{
	if (LobbyPlayerController)
	{
		LobbyPlayerController->Server_RequestSlotSelectionChange(NewSlotID);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 获取游戏状态
	CGameState = World->GetGameState<ACGameState>();
	if (!CGameState)
	{
		// 如果没有找到，设置定时器定期重试
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ULobbyWidget::ConfigureGameState, 1.f);
	}else
	{
		// 绑定玩家选择更新事件
		CGameState->OnPlayerSelectionUpdated.AddUObject(this, &ULobbyWidget::UpdatePlayerSelectionDisplay);
		// 初始化显示当前玩家选择
		UpdatePlayerSelectionDisplay(CGameState->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	// 清空所有槽位显示
	for (UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots)
	{
		SelectionSlot->UpdateSlotInfo("Empty");
	}

	// 重置角色选择项的选中状态
	for (UUserWidget* CharacterEntryAsWidget : CharacterSelectionTileView->GetDisplayedEntryWidgets())
	{
		if (UCharacterEntryWidget* CharacterEntryWidget = Cast<UCharacterEntryWidget>(CharacterEntryAsWidget))
		{
			CharacterEntryWidget->SetSelected(false);
		}
	}
	
	// 更新每个玩家的槽位显示
	for (const FPlayerSelection& PlayerSelection : PlayerSelections)
	{
		if (!PlayerSelection.IsValid())
			continue;
		// 更新槽位名称显示
		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());

		// 已选择的角色变成灰色让别人知道不能选了
		if (UCharacterEntryWidget* SelectedEntry = CharacterSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(PlayerSelection.GetCharacterDefinition()))
		{
			SelectedEntry->SetSelected(true);
		}

		// 如果是当前玩家，更新角色预览
		if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
		{
			UpdateCharacterDisplay(PlayerSelection);
		}
	}

	if (CGameState)
	{
		// 更新设置按钮是否可点击
		StartHeroSelectionButton->SetIsEnabled(CGameState->CanStartHeroSelection());
		// 更新设置开始按钮是否可点击
		StartMatchButton->SetIsEnabled(CGameState->CanStartMatch());
		// TODO: 凑齐十个秒开操作
		// if (CGameState->CanStartHeroSelection())
		// {
		// 	if (LobbyPlayerController)
		// 	{
		// 		// 请求服务器开始英雄选择流程
		// 		LobbyPlayerController->Server_StartHeroSelection();
		// 	}
		// }
	}
	// 更新队伍布局显示
	if (PlayerTeamLayoutWidget)
	{
		PlayerTeamLayoutWidget->UpdatePlayerSelection(PlayerSelections);
	}
}


void ULobbyWidget::StartHeroSelectionButtonClicked()
{
	if (LobbyPlayerController)
	{
		// 请求服务器开始英雄选择流程
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection()
{
	// 切换到英雄选择界面
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::CharacterDefinitionLoaded()
{
	TArray<UPDA_CharacterDefinition*> LoadedCharacterDefinitions;
	// 获取已加载的角色定义
	if (UCAssetManager::Get().GetLoadedCharacterDefinitions(LoadedCharacterDefinitions))
	{
		// 设置角色选择列表数据
		CharacterSelectionTileView->SetListItems(LoadedCharacterDefinitions);
	}
}

void ULobbyWidget::CharacterSelected(UObject* SelectedUObject)
{
	if (!MPlayerState)
	{
		MPlayerState = GetOwningPlayerState<AMPlayerState>();
	}

	if (!MPlayerState) return;
	
	// 获取选择的角色定义
	if (const UPDA_CharacterDefinition* SelectedCharacterDefinition = Cast<UPDA_CharacterDefinition>(SelectedUObject))
	{
		// 通知服务器更新角色选择
		MPlayerState->Server_SetSelectedCharacterDefinition(SelectedCharacterDefinition);
	}
}

void ULobbyWidget::SpawnCharacterDisplay()
{
	// 已经生成或者未定义角色展示类
	if (CharacterDisplay || !CharacterDisplayClass) return;

	// 设置预览角色的初始变换
	FTransform CharacterDisplayTransform = FTransform::Identity;

	// 获取玩家出生点位置
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart)
	{
		// 找到玩家出生点，将出生点设置为角色展示的初始变换
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}
	
	// 设置生成参数
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 生成预览角色
	CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, CharacterDisplayTransform, SpawnParams);
	// 设置玩家视角到预览角色
	GetOwningPlayer()->SetViewTarget(CharacterDisplay);
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
	if (!PlayerSelection.GetCharacterDefinition())
		return;

	// 配置角色预览
	CharacterDisplay->ConfigureWithCharacterDefinition(PlayerSelection.GetCharacterDefinition());
	// 清空现有技能列表
	AbilityListView->ClearListItems();
	// 获取技能映射
	if (const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* Abilities = PlayerSelection.GetCharacterDefinition()->GetAbilities())
	{
		// 配置技能列表
		AbilityListView->ConfigureAbilities(*Abilities);
	}
}

void ULobbyWidget::StartMatchButtonClicked()
{
	if (LobbyPlayerController)
	{
		// 请求服务器开始比赛
		LobbyPlayerController->Server_RequestStartMatch();
	}
}
