// 幻雨喜欢小猫咪


#include "Crunch/Private/Player/CPlayerController.h"

#include "CPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// 获取增强输入子系统（Enhanced Input Local Player Subsystem）
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (InputSubsystem)
	{
		// 移除现有的UI输入映射（避免重复添加）
		InputSubsystem->RemoveMappingContext(UIInputMapping);
		// 添加UI输入映射上下文（优先级为1）
		InputSubsystem->AddMappingContext(UIInputMapping, 1);
	}
	// 将基础InputComponent转换为增强输入组件
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		// 绑定商店切换输入动作
		EnhancedInputComponent->BindAction(
			ShopToggleInputAction,			// 输入动作资产引用
			ETriggerEvent::Triggered,			// 触发时机：按键时立即触发
			this,								// 目标对象：当前玩家控制器
			&ACPlayerController::ToggleShop		// 绑定的成员函数
		);
		// 绑定游戏菜单切换输入动作
		EnhancedInputComponent->BindAction(ToggleGameplayMenuAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleGameplayMenu);
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

void ACPlayerController::MatchFinished(AActor* ViewTarget, int WiningTeam)
{
	// 仅在服务器执行
	if (!HasAuthority())
		return;

	// 禁用玩家输入控制
	CPlayerCharacter->DisableInput(this);
	
	// 调用客户端RPC同步比赛结束状态
	Client_MatchFinished(ViewTarget, WiningTeam);
}

void ACPlayerController::Client_MatchFinished_Implementation(AActor* ViewTarget, int WiningTeam)
{
	// 切换视角到获胜目标，带有平滑过渡
	SetViewTargetWithBlend(ViewTarget, MatchFinishViewBlendTimeDuration);

	// 默认提示为胜利
	FString WinLoseMsg = TEXT("获胜");
	// 如果玩家队伍ID与获胜队伍不一致，则显示失败提示
	if (GetGenericTeamId().GetId() != WiningTeam)
	{
		WinLoseMsg = TEXT("失败");
	}

	// 设置游戏菜单标题为胜负提示
	GameplayWidget->SetGameplayMenuTitle(WinLoseMsg);
	
	// 延迟一段时间后显示胜负界面
	FTimerHandle ShowWinLoseStateTimerHandle;
	GetWorldTimerManager().SetTimer(ShowWinLoseStateTimerHandle, this, &ACPlayerController::ShowWinLoseState, MatchFinishViewBlendTimeDuration);
}

// void ACPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AActor* TargetActor, bool bCriticalHit)
// {
// 	if (!IsValid(TargetActor) || !NumberPopComponentClass || !IsLocalController())
// 		return;
//
// 	UNumberPopComponent_NiagaraText* DamageText = nullptr;
//     
// 	// 查找可复用组件
// 	for (UNumberPopComponent_NiagaraText* Pop : ActiveNumberPops)
// 	{
// 		if (Pop && Pop->GetOwner() == TargetActor)
// 		{
// 			DamageText = Pop;
// 			break;
// 		}
// 	}
//
// 	// 创建新组件或复用现有组件
// 	if (!DamageText)
// 	{
// 		DamageText = NewObject<UNumberPopComponent_NiagaraText>(TargetActor, NumberPopComponentClass);
// 		if (!DamageText)
// 		{
// #if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// 			UE_LOG(LogTemp, Error, TEXT("Niagara组件创建失败，内存不足或配置错误"));
// #endif
// 			return;
// 		}
// 		DamageText->RegisterComponent();
// 		ActiveNumberPops.Add(DamageText);
// 		TargetActor->OnDestroyed.AddDynamic(this, &ACPlayerController::HandleTargetActorDestroyed);
// 	}
// 	else if (!DamageText->IsRegistered())
// 	{
// 		DamageText->RegisterComponent();
// 	}
//
// 	// 设置显示参数
// 	FNumberPopRequest NumberPopRequest;
// 	NumberPopRequest.WorldLocation = TargetActor->GetActorLocation();
// 	NumberPopRequest.WorldLocation.Z += 200.f;
// 	NumberPopRequest.bIsCriticalDamage = bCriticalHit;
// 	NumberPopRequest.NumberToDisplay = DamageAmount;
//     
// 	DamageText->AddNumberPop(NumberPopRequest);
// 	// 确保传入的目标没被销毁并且设置了组件类
// 	// if (IsValid(TargetActor) && NumberPopComponentClass && IsLocalController())
// 	// {
// 	// 	FNumberPopRequest NumberPopRequest;
// 	// 	NumberPopRequest.WorldLocation = TargetActor->GetActorLocation();
// 	// 	NumberPopRequest.WorldLocation.Z += 200.f;
// 	// 	NumberPopRequest.bIsCriticalDamage = bCriticalHit;
// 	// 	NumberPopRequest.NumberToDisplay = DamageAmount;
// 	// 	// NumberPopComponent->AddNumberPop(NumberPopRequest);
// 	// 	UNumberPopComponent_NiagaraText* DamageText = NewObject<UNumberPopComponent_NiagaraText>(TargetActor, NumberPopComponentClass);
// 	// 	DamageText->RegisterComponent(); //动态创建的组件需要调用注册
// 	// 	DamageText->AddNumberPop(NumberPopRequest);
// 	// }
// }
void ACPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit, FGameplayTag DamageType)
{
	if (!IsValid(TargetCharacter) || !NumberPopComponentClass || !IsLocalController())
		return;

	// 获取目标Actor上的现有组件
	UNumberPopComponent_NiagaraText* DamageText = TargetCharacter->GetComponentByClass<UNumberPopComponent_NiagaraText>();

	// 不存在则创建并附加
	if (!DamageText)
	{
		DamageText = NewObject<UNumberPopComponent_NiagaraText>(TargetCharacter, NumberPopComponentClass);
		if (!DamageText)
		{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			UE_LOG(LogTemp, Error, TEXT("Niagara组件创建失败，内存不足或配置错误"));
#endif
			return;
		}
        
		DamageText->RegisterComponent(); // 注册组件
	}
	//修改~Begin
	// 新增随机偏移
	FVector BaseOffset(0, 0, 200);
	float RandomX = FMath::FRandRange(-100.0f, 100.0f);  // X轴随机偏移
	float RandomY = FMath::FRandRange(-100.0f, 100.0f);  // Y轴随机偏移
	float RandomZ = FMath::FRandRange(-20.0f, 50.0f);  // Z轴微调
	//修改~End
	// TODO:添加各种伤害类型的伤害数字特效
	// 设置显示参数
	FNumberPopRequest NumberPopRequest;
	NumberPopRequest.WorldLocation = TargetCharacter->GetActorLocation() + BaseOffset + FVector(RandomX, RandomY, RandomZ) ;
	// NumberPopRequest.WorldLocation = TargetCharacter->GetActorLocation() + FVector(0, 0, 200);
	NumberPopRequest.bIsCriticalDamage = bCriticalHit;
	NumberPopRequest.NumberToDisplay = DamageAmount;
    
	DamageText->AddNumberPop(NumberPopRequest, DamageType);
}

// void ACPlayerController::HandleTargetActorDestroyed(AActor* DestroyedActor)
// {
// 	// 遍历所有活跃的 Niagara 组件
// 	for (int32 i = ActiveNumberPops.Num() - 1; i >= 0; i--)
// 	{
// 		UNumberPopComponent_NiagaraText* PopComponent = ActiveNumberPops[i];
// 		if (PopComponent && PopComponent->GetOwner() == DestroyedActor)
// 		{
// 			// 清理组件
// 			PopComponent->UnregisterComponent(); // 解除注册
// 			PopComponent->MarkAsGarbage();       // 标记为垃圾回收
// 			ActiveNumberPops.RemoveAt(i);        // 从容器中移除
// 		}
// 	}
// }



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

void ACPlayerController::ToggleShop()
{
	// UE_LOG(LogTemp, Warning, TEXT("ToggleShop"))
	if (GameplayWidget)
	{
		GameplayWidget->ToggleShop();
	}
}

void ACPlayerController::ToggleGameplayMenu()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleGameplayMenu();
	}
}

void ACPlayerController::ShowWinLoseState()
{
	if (GameplayWidget)
	{
		GameplayWidget->ShowGameplayMenu();
	}
}
// void ACPlayerController::BeginDestroy()
// {
// 	for (UNumberPopComponent_NiagaraText* Pop : ActiveNumberPops)
// 	{
// 		if (Pop && Pop->IsValidLowLevel())
// 		{
// 			// 解绑目标Actor的 OnDestroyed 事件
// 			if (AActor* MyOwner = Pop->GetOwner())
// 			{
// 				MyOwner->OnDestroyed.RemoveAll(this); // 解绑所有与当前对象相关的委托
// 			}
//             
// 			Pop->UnregisterComponent();
// 			Pop->MarkAsGarbage();
// 		}
// 	}
// 	ActiveNumberPops.Empty();
//     
// 	Super::BeginDestroy();
// }
