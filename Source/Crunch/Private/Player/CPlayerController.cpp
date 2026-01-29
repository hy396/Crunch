// 幻雨喜欢小猫咪


#include "CPlayerController.h"
#include "UI/Gameplay/Minimap/UnitManager.h"

#include "CPlayerCharacter.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UI/Gameplay/GameplayWidget.h"
#include "UI/Gameplay/Chat/ChatWidget.h"
// #include "AsyncLoadingScreenLibrary.h"

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

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		// 移除现有的UI输入映射（避免重复添加）
		InputSubsystem->RemoveMappingContext(UIInputMapping);
		// 添加UI输入映射上下文（优先级为1）
		InputSubsystem->AddMappingContext(UIInputMapping, 1);
	}
	// 将基础InputComponent转换为增强输入组件
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 原ETriggerEvent::Triggered,
		// Started
		// 绑定商店切换输入动作
		EnhancedInputComponent->BindAction(
			ShopToggleInputAction,			// 输入动作资产引用
			ETriggerEvent::Triggered,			// 触发时机：按键时立即触发
			this,								// 目标对象：当前玩家控制器
			&ACPlayerController::ToggleShop		// 绑定的成员函数
		);
		// 绑定游戏菜单切换输入动作
		EnhancedInputComponent->BindAction(ToggleGameplayMenuAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleGameplayMenu);

		// 绑定聊天切换输入动作
		EnhancedInputComponent->BindAction(ToggleChatInputAction, ETriggerEvent::Triggered, this, &ACPlayerController::ToggleChat);

		// 绑定属性面板切换输入动作
		EnhancedInputComponent->BindAction(
			ToggleAttributePanelInputAction,
			ETriggerEvent::Triggered,
			this,
			&ACPlayerController::ToggleAttributePanel
		);
	
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

void ACPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	// UAsyncLoadingScreenLibrary::StopSeamlessLoadingScreen();
}

void ACPlayerController::Client_ShowCombatText_Implementation(int32 Amount, AActor* TargetActor, ECurrencyType CurrencyType)
{
	if (TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("显示战斗数字：%d 目标：%s 类型：%d"), Amount, *TargetActor->GetName(), (int32)CurrencyType)
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("无效的目标Actor"))
	}
	
	// 确保传入的目标没被销毁并且设置了组件类
	if (TargetActor && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetActor, DamageTextComponentClass);
		DamageText->RegisterComponent(); //动态创建的组件需要调用注册
		DamageText->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); //先附加到角色身上，使用角色位置
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //然后从角色身上分离，保证在一个位置播放完成动画
		DamageText->SetCurrencyText(Amount, CurrencyType); //设置显示的数字
	}
}

void ACPlayerController::Server_SendChatMessage_Implementation(const FString& Message, EChatChannelType ChannelType)
{
	// 验证消息不为空
	if (Message.IsEmpty() || Message.Len() > 100) // 限制消息长度
	{
		UE_LOG(LogTemp, Warning, TEXT("聘天消息为空或超长，已拒绝发送"));
		return;
	}

	// 获取发送者名称
	FString SenderName = GetPlayerState<APlayerState>()->GetPlayerName();
	if (SenderName.IsEmpty())
	{
		SenderName = TEXT("Unknown Player");
	}

	UE_LOG(LogTemp, Warning, TEXT("服务器接收到聊天消息：%s 发送者：%s 频道：%d"), *Message, *SenderName, (int32)ChannelType);

	// 创建聊天消息
	FChatMessage ChatMessage(SenderName, Message, ChannelType, GetGenericTeamId());

	// 根据频道类型决定发送范围
	UWorld* World = GetWorld();
	if (!World) return;

	int32 ReceiverCount = 0;
	for (TActorIterator<ACPlayerController> It(World); It; ++It)
	{
		ACPlayerController* PlayerController = *It;
		if (!PlayerController) continue;

		// 全体聊天：发送给所有玩家
		if (ChannelType == EChatChannelType::All)
		{
			PlayerController->Client_ReceiveChatMessage(ChatMessage);
			ReceiverCount++;
		}
		// 队友聊天：只发送给同队伍玩家
		else if (ChannelType == EChatChannelType::Team)
		{
			if (PlayerController->GetGenericTeamId() == GetGenericTeamId())
			{
				PlayerController->Client_ReceiveChatMessage(ChatMessage);
				ReceiverCount++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("聊天消息已发送给 %d 个玩家"), ReceiverCount);
}

bool ACPlayerController::Server_SendChatMessage_Validate(const FString& Message, EChatChannelType ChannelType)
{
	// 基本验证
	return !Message.IsEmpty() && Message.Len() <= 100;
}

void ACPlayerController::Client_ReceiveChatMessage_Implementation(const FChatMessage& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("客户端接收到聊天消息：%s 发送者：%s"), *Message.MessageContent, *Message.SenderName);
	
	if (GameplayWidget)
	{
		// 判断发送者类型
		bool bIsSelf = false;
		bool bIsTeammate = false;
		
		// 判断是否是自己发送的消息
		if (GetPlayerState<APlayerState>())
		{
			FString MyPlayerName = GetPlayerState<APlayerState>()->GetPlayerName();
			bIsSelf = (MyPlayerName == Message.SenderName);
		}
		
		// 如果不是自己，判断是否是队友
		if (!bIsSelf)
		{
			bIsTeammate = (GetGenericTeamId() == Message.SenderTeamId);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("发送者类型判断：自己=%s 队友=%s"), 
		       bIsSelf ? TEXT("true") : TEXT("false"), 
		       bIsTeammate ? TEXT("true") : TEXT("false"));
		
		// 同时显示临时消息和添加到聊天列表
		GameplayWidget->ShowTemporaryChatMessage(Message, bIsSelf, bIsTeammate);
		// 显示弹幕消息
		GameplayWidget->ShowBarrageMessage(Message, bIsSelf, bIsTeammate);
		
		UE_LOG(LogTemp, Warning, TEXT("将消息传递给GameplayWidget"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameplayWidget为空，无法显示聊天消息"));
	}
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

void ACPlayerController::RegisterUnitForMinimap(AActor* Unit)
{
	if (!Unit) return;

	// 获取单位管理器并注册单位
	if (UWorld* World = GetWorld())
	{
		if (UUnitManager* UnitManager = World->GetSubsystem<UUnitManager>())
		{
			UnitManager->RegisterUnit(Unit);
		}
	}
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
// 			break;F
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
void ACPlayerController::Client_ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit, FGameplayTag DamageType)
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
	FVector BaseOffset(0, 0, TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
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

void ACPlayerController::ToggleChat()
{
	if (GameplayWidget)
	{
		// 使用智能切换：在临时模式下直接进入正常聊天，否则正常切换
		GameplayWidget->SmartToggleChat();
	}
}

void ACPlayerController::ToggleAttributePanel()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleAttributePanel();
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

// IChatInterface 接口实现
void ACPlayerController::SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType)
{
	// 调用实际的RPC函数
	Server_SendChatMessage(Message, ChannelType);
}

void ACPlayerController::ReceiveChatMessageFromServer(const FChatMessage& Message)
{
	// 调用实际的RPC函数
	Client_ReceiveChatMessage(Message);
}

void ACPlayerController::UpdateTeamOnePlayerKillCount(int32 NewKillCount)
{
	if (IsLocalPlayerController())
	{
		OnTeamOnePlayerKillChanged.Broadcast(NewKillCount);
	}
}

void ACPlayerController::UpdateTeamTwoPlayerKillCount(int32 NewKillCount)
{
	if (IsLocalPlayerController())
	{
		OnTeamTwoPlayerKillChanged.Broadcast(NewKillCount);
	}
}
