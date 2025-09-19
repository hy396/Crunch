// 幻雨喜欢小猫咪

#include "UnitManager.h"
#include "MinimapManager.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Character/CCharacter.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/Core/TGameplayTags.h"
#include "Framework/StormCore.h"
#include "Player/CPlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

void UUnitManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

ACPlayerController* UUnitManager::GetLocalPlayerController() const
{
	// 在多人游戏中，获取本地玩家控制器（当前客户端的玩家控制器）
	UWorld* World = GetWorld();
	if (World && World->GetGameInstance())
	{
		// 获取本地玩家
		ULocalPlayer* LocalPlayer = World->GetGameInstance()->GetLocalPlayerByIndex(0);
		if (LocalPlayer)
		{
			// 通过本地玩家获取玩家控制器
			return Cast<ACPlayerController>(LocalPlayer->GetPlayerController(World));
		}
	}
	return nullptr;
}

void UUnitManager::RegisterUnit(AActor* Unit)
{
	if (!Unit) return;

	// 检查是否为StormCore（用于决定输赢的核心单位）
	// 如果是StormCore，我们不将其添加到小地图中
	if (Cast<AStormCore>(Unit))
	{
		return;
	}

	// 获取单位的能力系统组件
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Unit);
	if (!AbilitySystemInterface) return;

	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC) return;

	// 使用现有的IsHero函数检查单位是否为英雄
	bool bIsHero = UCAbilitySystemStatics::IsHero(Unit);
	
	// 如果不是英雄，那么就是小兵（不需要专门的小兵标签）
	bool bIsMinion = !bIsHero;

	// 只处理英雄和小兵单位
	if (!bIsHero && !bIsMinion) return;

	// 检查是否已注册
	for (const FUnitInfo& UnitInfo : RegisteredUnits)
	{
		if (UnitInfo.Unit.IsValid() && UnitInfo.Unit.Get() == Unit)
		{
			return; // 己注册
		}
	}

	// 添加到注册列表
	FGameplayTag UnitTypeTag = bIsHero ? TGameplayTags::Role_Hero : FGameplayTag::RequestGameplayTag("Unit.Type.Minion");
	RegisteredUnits.Emplace(Unit, UnitTypeTag);

	// 获取单位图标和颜色
	UTexture2D* Icon = nullptr;
	FLinearColor MarkerColor = FLinearColor::White;

	// 获取本地玩家控制器
	ACPlayerController* LocalPlayerController = GetLocalPlayerController();
	FGenericTeamId LocalTeamId = LocalPlayerController ? LocalPlayerController->GetGenericTeamId() : FGenericTeamId::NoTeam;

	// 获取单位的团队ID
	IGenericTeamAgentInterface* UnitTeamInterface = Cast<IGenericTeamAgentInterface>(Unit);
	FGenericTeamId UnitTeamId = UnitTeamInterface ? UnitTeamInterface->GetGenericTeamId() : FGenericTeamId::NoTeam;

	if (bIsHero)
	{
		// 对于英雄单位，根据敌我关系设置颜色
		if (LocalTeamId == UnitTeamId)
		{
			// 己方英雄
			if (LocalPlayerController && LocalPlayerController->GetPawn() == Unit)
			{
				// 自己的英雄使用绿色
				MarkerColor = FLinearColor(0.0f, 1.0f, 0.0f); // 绿色
			}
			else
			{
				// 队友英雄使用蓝色
				MarkerColor = FLinearColor(0.0f, 0.0f, 1.0f); // 蓝色
			}
		}
		else
		{
			// 敌方英雄使用红色
			MarkerColor = FLinearColor(1.0f, 0.0f, 0.0f); // 红色
		}
	}
	else if (bIsMinion)
	{
		// 小兵根据敌我关系设置颜色
		if (LocalTeamId == UnitTeamId)
		{
			// 己方小兵使用浅蓝色
			MarkerColor = FLinearColor(0.5f, 0.5f, 1.0f); // 浅蓝色
		}
		else
		{
			// 敌方小兵使用红色
			MarkerColor = FLinearColor(1.0f, 0.0f, 0.0f); // 红色
		}
	}

	// 通知小地图管理器添加单位
	if (UWorld* World = GetWorld())
	{
		// 使用SubsystemBlueprintLibrary获取EngineSubsystem，只需要传入类类型
		if (UMinimapManager* MinimapManager = Cast<UMinimapManager>(USubsystemBlueprintLibrary::GetEngineSubsystem(UMinimapManager::StaticClass())))
		{
			MinimapManager->AddUnitToMinimaps(Unit, Icon, MarkerColor);
		}
	}
}

void UUnitManager::UnregisterUnit(AActor* Unit)
{
	if (!Unit) return;

	// 从注册列表中移除
	for (int32 i = RegisteredUnits.Num() - 1; i >= 0; --i)
	{
		if (RegisteredUnits[i].Unit.IsValid() && RegisteredUnits[i].Unit.Get() == Unit)
		{
			RegisteredUnits.RemoveAt(i);
			break;
		}
	}

	// 通知小地图管理器移除单位
	if (UWorld* World = GetWorld())
	{
		// 使用SubsystemBlueprintLibrary获取EngineSubsystem，只需要传入类类型
		if (UMinimapManager* MinimapManager = Cast<UMinimapManager>(USubsystemBlueprintLibrary::GetEngineSubsystem(UMinimapManager::StaticClass())))
		{
			MinimapManager->RemoveUnitFromMinimaps(Unit);
		}
	}
}

void UUnitManager::UpdateUnitStatus(AActor* Unit)
{
	if (!Unit) return;

	// 通知小地图管理器更新单位状态
	if (UWorld* World = GetWorld())
	{
		// 使用SubsystemBlueprintLibrary获取EngineSubsystem，只需要传入类类型
		if (UMinimapManager* MinimapManager = Cast<UMinimapManager>(USubsystemBlueprintLibrary::GetEngineSubsystem(UMinimapManager::StaticClass())))
		{
			MinimapManager->UpdateUnitStatusOnMinimaps(Unit);
		}
	}
}