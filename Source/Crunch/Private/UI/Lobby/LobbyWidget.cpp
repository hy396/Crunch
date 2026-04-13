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
#include "UI/Gameplay/Chat/ChatWidget.h"
#include "UI/Gameplay/Chat/ChatMessageItemWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

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

	// 在玩家加入的时候自动更新了按钮是否可点击
	// 绑定开始比赛按钮事件
	// StartMatchButton->SetIsEnabled(false);
	StartMatchButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartMatchButtonClicked);

	// 绑定退出大厅按钮事件
	if (LeaveLobbyButton)
	{
		LeaveLobbyButton->OnClicked.AddDynamic(this, &ULobbyWidget::LeaveLobbyButtonClicked);
	}

	// 异步加载角色定义数据
	UCAssetManager::Get().LoadCharacterDefinitions(FStreamableDelegate::CreateUObject(this, &ULobbyWidget::CharacterDefinitionLoaded));

	if (CharacterSelectionTileView)
	{
		// 绑定角色选择事件
		CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ULobbyWidget::CharacterSelected);
	}
	
	SpawnCharacterDisplay(); // 生成角色预览Actor

	// 配置聊天组件
	if (ChatWidget)
	{
		// 大厅聊天框一直显示，不需要隐藏
		ChatWidget->SetVisibility(ESlateVisibility::Visible);
		ChatWidget->ShowChatWidget();
		// 配置大厅模式的聊天频道（移除全体聊天）
		ConfigureLobbyModeChat();
		UE_LOG(LogTemp, Warning, TEXT("大厅聊天组件初始化完成"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ChatWidget为空，无法初始化聊天功能"));
	}
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


		// TODO : 2026/4/13，允许重复选择，当角色足够多的时候去掉注释, 该作用域下被注释掉的代码就是原逻辑
		// 如果是当前玩家，更新角色预览并标灰该英雄
		if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
		{
			UpdateCharacterDisplay(PlayerSelection);

			// 仅本地玩家自己选中的英雄在本地 UI 中标灰
			if (UCharacterEntryWidget* SelectedEntry = CharacterSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(PlayerSelection.GetCharacterDefinition()))
			{
				SelectedEntry->SetSelected(true);
			}
		}
		// // 已选择的角色变成灰色让别人知道不能选了
		// if (UCharacterEntryWidget* SelectedEntry = CharacterSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(PlayerSelection.GetCharacterDefinition()))
		// {
		// 	SelectedEntry->SetSelected(true);
		// }

		// // 如果是当前玩家，更新角色预览
		// if (PlayerSelection.IsForPlayer(GetOwningPlayerState()))
		// {
		// 	UpdateCharacterDisplay(PlayerSelection);
		// }
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

void ULobbyWidget::LeaveLobbyButtonClicked()
{
	if (LobbyPlayerController)
	{
		// 请求退出大厅并返回主菜单
		LobbyPlayerController->LeaveLobby();
	}
}

void ULobbyWidget::FocusChatInput()
{
	if (ChatWidget)
	{
		ChatWidget->FocusChatInput();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ChatWidget为空，无法设置输入焦点"));
	}
}

void ULobbyWidget::ShowBarrageMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
	UE_LOG(LogTemp, Warning, TEXT("大厅弹幕 - 显示弹幕消息：%s"), *Message.MessageContent);
	
	if (!HeroSelectionRoot || !ChatMessageItemClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HeroSelectionRoot或ChatMessageItemClass为空，无法显示弹幕"));
		return;
	}
	
	// 获取实际屏幕大小
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
	if (ViewportSize.X <= 0 || ViewportSize.Y <= 0)
	{
		// 如果获取失败，使用默认值
		ViewportSize = FVector2D(1920.0f, 1080.0f);
		UE_LOG(LogTemp, Warning, TEXT("无法获取屏幕大小，使用默认值"));
	}
	
	// 创建弹幕消息项
	UChatMessageItemWidget* BarrageItem = CreateWidget<UChatMessageItemWidget>(this, ChatMessageItemClass);
	if (BarrageItem)
	{
		// 将弹幕添加到英雄选择根面板
		UCanvasPanelSlot* CanvasSlot = HeroSelectionRoot->AddChildToCanvas(BarrageItem);
		if (CanvasSlot)
		{
			// 计算弹幕位置（居中区域 + 随机浮动）
			float CenterY = ViewportSize.Y * 0.5f; // 屏幕中心
			float FloatRange = ViewportSize.Y * 0.3f; // 上下浮动范围（屏幕高度的30%）
			
			// 使用随机数生成Y位置
			float RandomOffset = FMath::RandRange(-FloatRange * 0.5f, FloatRange * 0.5f);
			float BarrageY = CenterY + RandomOffset;
			
			// 确保弹幕不会超出屏幕边界
			float MinY = ViewportSize.Y * 0.1f; // 距离顶部10%
			float MaxY = ViewportSize.Y * 0.9f; // 距离底部10%
			BarrageY = FMath::Clamp(BarrageY, MinY, MaxY);
			
			// 设置初始位置（屏幕右侧外更远，符合弹幕风格）和大小
			float StartX = ViewportSize.X + 200.0f; // 屏幕外侧200像素
			CanvasSlot->SetPosition(FVector2D(StartX, BarrageY));
			CanvasSlot->SetSize(FVector2D(400.0f, 30.0f));
			
			// 设置为弹幕模式
			BarrageItem->SetAsBarrageMode(Message, bIsSelf, bIsTeammate, HeroSelectionRoot);
			
			UE_LOG(LogTemp, Warning, TEXT("大厅弹幕消息创建成功，屏幕大小：(%.2f, %.2f)，弹幕位置：(%.2f, %.2f)"), 
			       ViewportSize.X, ViewportSize.Y, StartX, BarrageY);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("大厅弹幕消息项创建失败"));
	}
}

void ULobbyWidget::ConfigureLobbyModeChat()
{
	if (!ChatWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("ChatWidget为空，无法配置大厅模式聊天"));
		return;
	}
	
	// 获取聊天频道下拉框
	UComboBoxString* ChannelComboBox = ChatWidget->GetChannelComboBox();
	if (!ChannelComboBox)
	{
		UE_LOG(LogTemp, Error, TEXT("无法获取ChannelComboBox，无法配置大厅模式聊天"));
		return;
	}
	
	// 清除所有选项
	ChannelComboBox->ClearOptions();
	
	// 只添加队伍聊天选项
	ChannelComboBox->AddOption(TEXT("队伍聊天"));
	
	// 设置默认选中队伍聊天
	ChannelComboBox->SetSelectedOption(TEXT("队伍聊天"));
	
	// 设置大厅模式：发送消息后不自动隐藏聊天框
	ChatWidget->SetAutoHideAfterSend(false);
	
	UE_LOG(LogTemp, Warning, TEXT("大厅模式聊天配置完成，已移除全体聊天选项，只保留队伍聊天，且发送后不隐藏"));
}
