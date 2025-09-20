// 幻雨喜欢小猫咪

#include "TeamPlayerPortraitsWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "PlayerPortraitWidget.h"
#include "Player/MPlayerState.h"
#include "Character/CCharacter.h"
#include "Framework/CGameState.h"
#include "Components/VerticalBox.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Components/PanelWidget.h"

void UTeamPlayerPortraitsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 初始化团队头像显示
	InitializeTeamPortraits();
}

void UTeamPlayerPortraitsWidget::InitializeTeamPortraits()
{
	// 获取本地玩家状态
	if (const APawn* Pawn = GetOwningPlayerPawn())
	{
		LocalPlayerState = Cast<AMPlayerState>(Pawn->GetPlayerState());
	}
	PlayerPortraitContainer->ClearChildren();
	EnemyPortraitContainer->ClearChildren();
	// 清空跟踪的玩家状态数组
	TrackedPlayerStates.Empty();
	// 获取当前游戏状态
	ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
	// int32 ActorPlayerCount = 0;
	// 获取所有玩家状态
	for (TActorIterator<AMPlayerState> It(GetWorld()); It; ++It)
	{
		AMPlayerState* PlayerState = *It;
		if (!PlayerState) continue;
		// 创建头像控件
		UPlayerPortraitWidget* NewPortraitWidget = CreateWidget<UPlayerPortraitWidget>(this, PlayerPortraitWidgetClass);
		// 从玩家角色获取能力系统组件
		if (APawn* PlayerPawn = PlayerState->GetPawn())
		{
			if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerPawn))
			{
				// 判断是否为同队玩家
				bool bIsSameTeam = false;
				if (LocalPlayerState && PlayerState)
				{
					// 使用已有的队伍ID判断方法
					bIsSameTeam = LocalPlayerState->GetTeamIdBasedOnSlot() == PlayerState->GetTeamIdBasedOnSlot();
				}
				if (LocalPlayerState && PlayerState == LocalPlayerState)
				{
					// 自己的头像
					NewPortraitWidget->SetAscAndPortrait(AbilitySystemComponent, PlayerState->GetPlayerSelection().GetCharacterDefinition(), true, true);
				}else if (bIsSameTeam)
				{
					// 队友的头像
					NewPortraitWidget->SetAscAndPortrait(AbilitySystemComponent, PlayerState->GetPlayerSelection().GetCharacterDefinition(), true, false);
				}else
				{
					// 敌方的头像
					NewPortraitWidget->SetAscAndPortrait(AbilitySystemComponent, PlayerState->GetPlayerSelection().GetCharacterDefinition(), false, false);
				}
				// 根据是否为同队决定添加到哪个容器
				if (bIsSameTeam)
				{
					PlayerPortraitContainer->AddChild(NewPortraitWidget);
				}
				else
				{
					EnemyPortraitContainer->AddChild(NewPortraitWidget);
				}
				// 将玩家状态添加到跟踪数组中
				TrackedPlayerStates.Add(PlayerState);
				// ++ActorPlayerCount;
			}
		}
	}
	if (CGameState && TrackedPlayerStates.Num() != CGameState->GetPlayerSelection().Num())
	{
		// 启动定时器，定期检查并刷新
		if (UWorld* World = GetWorld())
		{
			if (RefreshTimerHandle.IsValid())
			{
				World->GetTimerManager().ClearTimer(RefreshTimerHandle);
			}
			World->GetTimerManager().SetTimer(RefreshTimerHandle, this, &UTeamPlayerPortraitsWidget::OnTimerCallback, 1.0f, true);
		}
	}
}

void UTeamPlayerPortraitsWidget::OnTimerCallback()
{
	// PlayerPortraitContainer->ClearChildren();
	// EnemyPortraitContainer->ClearChildren();
	// 获取当前游戏状态
	ACGameState* CGameState = GetWorld()->GetGameState<ACGameState>();
	// 获取所有玩家状态
	for (TActorIterator<AMPlayerState> It(GetWorld()); It; ++It)
	{
		AMPlayerState* PlayerState = *It;
		if (!PlayerState) continue;
		if (TrackedPlayerStates.Contains(PlayerState))
		{
			continue;
		}
		// 创建头像控件
		UPlayerPortraitWidget* NewPortraitWidget = CreateWidget<UPlayerPortraitWidget>(this, PlayerPortraitWidgetClass);
		// 从玩家角色获取能力系统组件
		if (APawn* PlayerPawn = PlayerState->GetPawn())
		{
			if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerPawn))
			{
				// 判断是否为同队玩家
				bool bIsSameTeam = false;
				if (LocalPlayerState && PlayerState)
				{
					// 使用已有的队伍ID判断方法
					bIsSameTeam = LocalPlayerState->GetTeamIdBasedOnSlot() == PlayerState->GetTeamIdBasedOnSlot();
				}
				if (LocalPlayerState && PlayerState == LocalPlayerState)
				{
					// 自己的头像
					NewPortraitWidget->SetAscAndPortrait(AbilitySystemComponent, PlayerState->GetPlayerSelection().GetCharacterDefinition(), true, true);
				}else if (bIsSameTeam)
				{
					// 队友的头像
					NewPortraitWidget->SetAscAndPortrait(AbilitySystemComponent, PlayerState->GetPlayerSelection().GetCharacterDefinition(), true, false);
				}else
				{
					// 敌方的头像
					NewPortraitWidget->SetAscAndPortrait(AbilitySystemComponent, PlayerState->GetPlayerSelection().GetCharacterDefinition(), false, false);
				}
				// 根据是否为同队决定添加到哪个容器
				if (bIsSameTeam)
				{
					PlayerPortraitContainer->AddChild(NewPortraitWidget);
				}
				else
				{
					EnemyPortraitContainer->AddChild(NewPortraitWidget);
				}
				// 将玩家状态添加到跟踪数组中
				TrackedPlayerStates.Add(PlayerState);
			}
		}
	}
	if (CGameState && TrackedPlayerStates.Num() == CGameState->GetPlayerSelection().Num())
	{
		// 清除定时器
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(RefreshTimerHandle);
		}
	}
}
