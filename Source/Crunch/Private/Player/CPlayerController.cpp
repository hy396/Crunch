// 幻雨喜欢小猫咪


#include "Crunch/Private/Player/CPlayerController.h"

#include "CPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "UI/Gameplay/GameplayWidget.h"

ACPlayerController::ACPlayerController()
{
	bReplicates = true;
	// 创建数字弹出组件
	NumberPopComponent = CreateDefaultSubobject<UNumberPopComponent_NiagaraText>(TEXT("NumberPopComponent"));
}

void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
		CPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void ACPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);
	CPlayerCharacter = Cast<ACPlayerCharacter>(P);
	if (CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		// 创建UI
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamID);
}

void ACPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter,
	bool bCriticalHit)
{
	// 确保传入的目标没被销毁并且设置了组件类
	if (IsValid(TargetCharacter))// && IsLocalController())
	{
		FNumberPopRequest NumberPopRequest;
		NumberPopRequest.WorldLocation = TargetCharacter->GetActorLocation();
		NumberPopRequest.WorldLocation.Z += 200.f;
		NumberPopRequest.bIsCriticalDamage = bCriticalHit;
		NumberPopRequest.NumberToDisplay = DamageAmount;
		NumberPopComponent->AddNumberPop(NumberPopRequest);
		// if (TargetCharacter->Implements<UCombatInterface>())
		// {
		// 	ICombatInterface::Execute_AddNiagaraText(TargetCharacter,NumberPopRequest);
		// }
	}
}

void ACPlayerController::SpawnGameplayWidget()
{
	// 检查当前玩家控制器是否是本地玩家控制器
	if (!IsLocalPlayerController()) return;
	if (GameplayWidgetClass)
	{
		GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
		if (GameplayWidget)
		{
			// 添加到视口中
			GameplayWidget->AddToViewport();
			// 将技能数据传递给UI
			GameplayWidget->ConfigureAbilities(CPlayerCharacter->GetAbilities());
		}
	}
}
