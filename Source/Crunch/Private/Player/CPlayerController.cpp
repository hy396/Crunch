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
	// NumberPopComponent = CreateDefaultSubobject<UNumberPopComponent_NiagaraText>(TEXT("NumberPopComponent"));
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

void ACPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AActor* TargetActor, bool bCriticalHit)
{
	if (!IsValid(TargetActor) || !NumberPopComponentClass || !IsLocalController())
		return;

	UNumberPopComponent_NiagaraText* DamageText = nullptr;
    
	// 查找可复用组件
	for (UNumberPopComponent_NiagaraText* Pop : ActiveNumberPops)
	{
		if (Pop && Pop->GetOwner() == TargetActor)
		{
			DamageText = Pop;
			break;
		}
	}

	// 创建新组件或复用现有组件
	if (!DamageText)
	{
		DamageText = NewObject<UNumberPopComponent_NiagaraText>(TargetActor, NumberPopComponentClass);
		if (!DamageText)
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Error, TEXT("Niagara组件创建失败，内存不足或配置错误"));
#endif
			return;
		}
		DamageText->RegisterComponent();
		ActiveNumberPops.Add(DamageText);
		TargetActor->OnDestroyed.AddDynamic(this, &ACPlayerController::HandleTargetActorDestroyed);
	}
	else if (!DamageText->IsRegistered())
	{
		DamageText->RegisterComponent();
	}

	// 设置显示参数
	FNumberPopRequest NumberPopRequest;
	NumberPopRequest.WorldLocation = TargetActor->GetActorLocation();
	NumberPopRequest.WorldLocation.Z += 200.f;
	NumberPopRequest.bIsCriticalDamage = bCriticalHit;
	NumberPopRequest.NumberToDisplay = DamageAmount;
    
	DamageText->AddNumberPop(NumberPopRequest);
	// 确保传入的目标没被销毁并且设置了组件类
	// if (IsValid(TargetActor) && NumberPopComponentClass && IsLocalController())
	// {
	// 	FNumberPopRequest NumberPopRequest;
	// 	NumberPopRequest.WorldLocation = TargetActor->GetActorLocation();
	// 	NumberPopRequest.WorldLocation.Z += 200.f;
	// 	NumberPopRequest.bIsCriticalDamage = bCriticalHit;
	// 	NumberPopRequest.NumberToDisplay = DamageAmount;
	// 	// NumberPopComponent->AddNumberPop(NumberPopRequest);
	// 	UNumberPopComponent_NiagaraText* DamageText = NewObject<UNumberPopComponent_NiagaraText>(TargetActor, NumberPopComponentClass);
	// 	DamageText->RegisterComponent(); //动态创建的组件需要调用注册
	// 	DamageText->AddNumberPop(NumberPopRequest);
	// }
}

void ACPlayerController::HandleTargetActorDestroyed(AActor* DestroyedActor)
{
	// 遍历所有活跃的 Niagara 组件
	for (int32 i = ActiveNumberPops.Num() - 1; i >= 0; i--)
	{
		UNumberPopComponent_NiagaraText* PopComponent = ActiveNumberPops[i];
		if (PopComponent && PopComponent->GetOwner() == DestroyedActor)
		{
			// 清理组件
			PopComponent->UnregisterComponent(); // 解除注册
			PopComponent->MarkAsGarbage();       // 标记为垃圾回收
			ActiveNumberPops.RemoveAt(i);        // 从容器中移除
		}
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

void ACPlayerController::BeginDestroy()
{
	for (UNumberPopComponent_NiagaraText* Pop : ActiveNumberPops)
	{
		if (Pop && Pop->IsValidLowLevel())
		{
			// 解绑目标Actor的 OnDestroyed 事件
			if (AActor* MyOwner = Pop->GetOwner())
			{
				MyOwner->OnDestroyed.RemoveAll(this); // 解绑所有与当前对象相关的委托
			}
            
			Pop->UnregisterComponent();
			Pop->MarkAsGarbage();
		}
	}
	ActiveNumberPops.Empty();
    
	Super::BeginDestroy();
}
