// 幻雨喜欢小猫咪


#include "CCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Crunch/Crunch.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GAS/Core/TGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Sight.h"
#include "Player/CPlayerController.h"
#include "UI/Gameplay/OverHeadStatsGauge.h"

ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// 禁用网格的碰撞功能
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 忽略弹簧臂的碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SPRING_ARM, ECR_Ignore);
	// 忽略目标的碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_TARGET, ECR_Ignore);

	// 👇 新增：保存默认重力缩放值
	DefaultGravityScale = GetCharacterMovement()->GravityScale;
	
	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("CAbilitySystemComponent"));
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("CAttributeSet"));
	//CAbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制,经过我的测试本来就是true
	
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComponent"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());

	// 绑定GAS属性改变委托
	BindGASChangeDelegates();

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Perception Stimuli Source Component");

	// 无论是否渲染都Tick和刷新骨骼变换 - 用于专用服务器上的骨骼更新
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	// 创建数字弹出组件
	// NumberPopComponent = CreateDefaultSubobject<UNumberPopComponent_NiagaraText>(TEXT("NumberPopComponent"));

}

void ACCharacter::ServerSideInit()
{
	// 设置当前角色作为Owner和Avatar，用于后续的能力和效果应用
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent->ServerSideInit();
	// if (CAttributeSet)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Name:%s,Health:%f"), *GetName(),CAttributeSet->GetMaxHealth())
	// }
}

void ACCharacter::ClientSideInit()
{
	// 设置当前角色作为Owner和Avatar，用于后续的能力和效果应用
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

bool ACCharacter::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void ACCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamID);
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& ACCharacter::GetAbilities() const
{
	return CAbilitySystemComponent->GetAbilities();
}

void ACCharacter::SetOverHeadWidgetColor()
{
	// 将头顶UI组件的用户控件对象转换为UOverHeadStatsGauge类型

	// 确保在Owner变化时也刷新团队状态显示
	if (UOverHeadStatsGauge* OverheadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject()))
	{
		APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
		if (LocalPlayerPawn && LocalPlayerPawn->GetClass()->ImplementsInterface(UGenericTeamAgentInterface::StaticClass()))
		{
			if (const IGenericTeamAgentInterface* LocalTeamInterface = Cast<IGenericTeamAgentInterface>(LocalPlayerPawn))
			{
				// UE_LOG(LogTemp, Warning, TEXT("本地玩家 TeamID: %u"), LocalTeamInterface->GetGenericTeamId().GetId());
				// UE_LOG(LogTemp, Warning, TEXT("%s的当前角色 TeamID: %u"), *GetName(),GetGenericTeamId().GetId());
				// UE_LOG(LogTemp, Warning, TEXT("态度: %s"), *UEnum::GetValueAsString(GetTeamAttitudeTowards(*LocalPlayerPawn)));
				OverheadStatsGauge->SetHealthBarColor(GetTeamAttitudeTowards(*LocalPlayerPawn));
			}
		}
	}
}

FVector ACCharacter::GetCaptureLocalPosition() const
{
	return HeadshotCaptureLocalPosition;
}

FRotator ACCharacter::GetCaptureLocalRotation() const
{
	return HeadshotCaptureLocalRotation;
}

// void ACCharacter::AddNiagaraText_Implementation(const FNumberPopRequest& NewRequest)
// {
// 	// 添加特效文字
// 	NumberPopComponent->AddNumberPop(NewRequest);
// }

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 👇 新增：保存默认重力缩放值（确保在所有初始化完成后）
	DefaultGravityScale = GetCharacterMovement()->GravityScale;
	ConfigureOverHeadStatusWidget();

	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	// 注册视觉感知，建议在BeginPlay中注册，如果在构造函数中注册那么可能会因为组件还未初始化而失败
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	//UE_LOG(LogTemp, Warning, TEXT("ACCharacter::BeginPlay,%hhd"),GetIsReplicated());
	//UE_LOG(LogTemp, Warning, TEXT("True是：,%hhd"),true);
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}

void ACCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
}

void ACCharacter::Server_SendGameplayEventToSelf_Implementation(const FGameplayTag& EventTag,
	const FGameplayEventData& EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

/**
 * @brief 验证在服务器端向自身发送游戏事件的操作是否有效
 */
bool ACCharacter::Server_SendGameplayEventToSelf_Validate(const FGameplayTag& EventTag,
                                                          const FGameplayEventData& EventData)
{
	// 返回 true,表示操作有效
	return true;
}

void ACCharacter::UpgradeAbilityWithInputID(ECAbilityInputID InputID)
{
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->Server_UpgradeAbilityWithID(InputID);
	}
}

void ACCharacter::BindGASChangeDelegates()
{
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).AddUObject(this, &ACCharacter::DeathTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Stun).AddUObject(this, &ACCharacter::StunTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Aim).AddUObject(this, &ACCharacter::AimTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Focus).AddUObject(this, &ACCharacter::FocusTagUpdated);
		CAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Status::Phasing, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACCharacter::PhasingTagUpdated);
		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ACCharacter::MoveSpeedUpdated);
		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ACCharacter::MaxHealthUpdated);
		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ACCharacter::MaxManaUpdated);
		CAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMoveAccelerationAttribute()).AddUObject(this, &ACCharacter::MoveSpeedAccelerationUpdated);
	}
}

void ACCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	// 标签数量不为0时，死亡。为0则复活。
	if (NewCount != 0)
	{
		StartDeathSequence();
	}else
	{
		Respawn();
	}
}

void ACCharacter::StunTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (IsDead()) return;

	if (NewCount != 0)
	{
		OnStun();
		PlayAnimMontage(StunMontage);
	}else
	{
		OnRecoverFromStun();
		StopAnimMontage(StunMontage);
	}
}

void ACCharacter::AimTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	SetIsAiming(NewCount != 0);
}

void ACCharacter::FocusTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	bIsInFocusMode = NewCount > 0;
}

void ACCharacter::PhasingTagUpdated(const FGameplayTag GameplayTag, int32 NewCount)
{
	bIsPhasing = (NewCount > 0);

	// 如果进入相位状态，则禁用与Pawn，胶囊体的碰撞，否则启用碰撞
	if (bIsPhasing)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ACCharacter::SetIsAiming(bool bIsAiming)
{
	// 设置角色是否使用控制器的Yaw轴旋转（用于瞄准）
	bUseControllerRotationYaw = bIsAiming;
    
	// 设置角色移动组件是否根据移动方向自动调整旋转（非瞄准状态下启用）
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
	OnAimStateChanged(bIsAiming);
}

void ACCharacter::OnAimStateChanged(bool bIsAiming)
{
	// 子类中重写
}
void ACCharacter::MoveSpeedUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ACCharacter::MoveSpeedAccelerationUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxAcceleration = Data.NewValue;
}

void ACCharacter::MaxHealthUpdated(const FOnAttributeChangeData& Data)
{
	if (IsValid(CAttributeSet))
	{
		CAttributeSet->RescaleHealth();
	}
}

void ACCharacter::MaxManaUpdated(const FOnAttributeChangeData& Data)
{
	if (IsValid(CAttributeSet))
	{
		CAttributeSet->RescaleMana();
	}
}

void ACCharacter::OnStun()
{
}

void ACCharacter::OnRecoverFromStun()
{
}

void ACCharacter::ConfigureOverHeadStatusWidget()
{
	// 只在客户端执行，服务器端不需要处理UI
	if (HasAuthority())
	{
		return;
	}
	
	// 检查头顶UI组件是否存在，如果不存在则直接返回
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	// 如果角色由本地玩家控制
	if (IsLocallyControlledByPlayer())
	{
		// 隐藏头顶UI组件
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	// 将头顶UI组件的用户控件对象转换为UOverHeadStatsGauge类型
	UOverHeadStatsGauge* OverheadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatsGauge)
	{
		// 使用能力系统组件配置头顶统计量表
		OverheadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
		
		// 尝试获取玩家状态来设置玩家名称
		if (!TrySetPlayerName(OverheadStatsGauge))
		{
			// 如果未能成功设置玩家名称，启动定时器定期尝试
			GetWorldTimerManager().SetTimer(
				PlayerNameUpdateTimerHandle,     // 定时器句柄
				[this, OverheadStatsGauge]() {   // Lambda表达式捕获this和OverheadStatsGauge
					if (TrySetPlayerName(OverheadStatsGauge))
					{
						// 如果成功设置玩家名称，清除定时器
						GetWorldTimerManager().ClearTimer(PlayerNameUpdateTimerHandle);
					}
				},
				1.0f,    // 每秒执行一次
				true     // 循环执行
			);
		}
		
		// TODO: 获取本地玩家角色
		//if (!HasAuthority()) 
		//{
			// 获取到本地玩家角色
			APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
			if (LocalPlayerPawn && LocalPlayerPawn->GetClass()->ImplementsInterface(UGenericTeamAgentInterface::StaticClass()))
			{
				// 获取本地玩家角色的GenericTeamAgentInterface接口
				const IGenericTeamAgentInterface* LocalTeamInterface = Cast<IGenericTeamAgentInterface>(LocalPlayerPawn);
				if (LocalTeamInterface)
				{
					// 设置头顶UI组件的血条颜色
					OverheadStatsGauge->SetHealthBarColor(GetTeamAttitudeTowards(*LocalPlayerPawn));
				}
			}
		//}
		// 设置头顶UI颜色
		//	SetOverHeadWidgetColor();
		//}

		// 显示头顶UI组件
		OverHeadWidgetComponent->SetHiddenInGame(false);

		// 清除之前的定时器,防止重复调用
		GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);

		// 设置新的定时器，周期性调用 UpdateHeadGaugeVisibility 方法
		// 用于持续检测角色与玩家之间的距离并更新头顶UI可见性状态
		GetWorldTimerManager().SetTimer(
			HeadStatGaugeVisibilityUpdateTimerHandle,     // 定时器句柄
			this,											 // 绑定对象
			&ACCharacter::UpdateHeadGaugeVisibility,		 // 每一定时间调用的函数
			HeadStatGaugeVisibilityCheckUpdateGap,			 // 更新间隔时间（秒）
			true											 // 是否循环调用true为循环，false为单次
		);
	}
}

void ACCharacter::UpdateHeadGaugeVisibility()
{
	// 获取本地玩家的Pawn角色，用于计算与当前角色的距离
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		// 计算当前角色与本地玩家之间的平方距离
		float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());

		// 如果距离超过设定的可见范围，则隐藏头顶状态条组件
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatGaugeVisibilityRangeSquared);
	}
}

void ACCharacter::SetStatusGaugeEnabled(bool bIsEnabled)
{
	// 清除定时器
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	if (bIsEnabled)
	{
		// 启动头顶血条
		ConfigureOverHeadStatusWidget();
	}else
	{
		// 关闭头顶血条
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

bool ACCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(TGameplayTags::Stats_Dead);
}

void ACCharacter::RespawnImmediately()
{
	// 仅在服务器上执行：移除所有带有“死亡”标签的激活效果，实现立即复活
	if (HasAuthority())
	{
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(TGameplayTags::Stats_Dead));
	}
}

void ACCharacter::DeathMontageFinished()
{
	if (IsDead())
	{
		SetRagdollEnabled(true);
	}
}

void ACCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // 从父组件分离网格，但保持世界变换不变
		GetMesh()->SetSimulatePhysics(true); // 启用物理模拟
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); // 仅启用物理碰撞
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false); // 禁用物理模拟
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 禁用碰撞
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); // 将网格重新附加到根组件，保持相对变换
		GetMesh()->SetRelativeTransform(MeshRelativeTransform); // 设置网格的相对变换
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		// 获取死亡蒙太奇的持续时间
		float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &ACCharacter::DeathMontageFinished, MontageDuration + DeathMontageFinishTimeShift);
	}
}

void ACCharacter::StartDeathSequence()
{
	OnDead();

	// 取消激活的技能
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->CancelAllAbilities();
	}
	
	// 播放死亡动画
	PlayDeathAnimation();
	// 关闭头顶血条
	SetStatusGaugeEnabled(false);
	// 禁用移动
	// GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	// 禁用碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// TODO:2025/12/30修改
	// 禁用碰撞（改为忽略Pawn和WorldDynamic，但仍保持WorldStatic以防止跌落）
	// GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	// TODO:2025/12/30修改
	// 👇 第1步：先清空移动速度和物理
    GetCharacterMovement()->StopMovementImmediately();
	// 👇 新增：禁用重力，让尸体悬空
    GetCharacterMovement()->GravityScale = 0.0f;
	// 死掉后禁用感知
	SetAIPerceptionStimuliSourceEnabled(false);
	// UE_LOG(LogTemp, Warning, TEXT("%s：狗带"),*GetName())
}

void ACCharacter::Respawn()
{
	OnRespawn();
	// 启用感知
	SetAIPerceptionStimuliSourceEnabled(true);
	// 关闭布娃娃
	SetRagdollEnabled(false);
	// 开启血条
	SetStatusGaugeEnabled(true);
	// 👇 新增：恢复重力
	// 👇 修改：使用保存的默认重力值，而不是硬编码1.0f
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	// 开启移动
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	// 开启碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 停止所有蒙太奇
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);

	// 如果当前对象具有权限并且控制器存在
	if (HasAuthority() && GetController())
	{
		// 获取控制器的起始位置对象指针
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
    
		// 检查起始位置对象是否有效
		if (StartSpot.IsValid())
		{
			// 将当前对象的位置和姿态设置为起始位置对象的位置和姿态
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}
	
	// 应用全属性
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->ApplyFullStatEffect();
	}
	// UE_LOG(LogTemp, Warning, TEXT("%s：复活"),*GetName())
}

void ACCharacter::OnDead()
{
}

void ACCharacter::OnRespawn()
{
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	// 原代码
	// TeamID = NewTeamID;

	// 新代码 2026/1/29
	if (TeamID == NewTeamID)
	{
		return;
	}

	TeamID = NewTeamID;

	// 服务器上立即更新表现
	if (HasAuthority())
	{
		SetOverHeadWidgetColor();
	}
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void ACCharacter::RequestChangeTeamID(uint8 NewTeamID)
{
	// 如果当前已经在服务器（例如 Listen Server）
	// 可以直接修改 TeamID
	if (HasAuthority())
	{
		// 1. 设置 Character 的 TeamID
        SetGenericTeamId(FGenericTeamId(NewTeamID));
        
        // 2. 同步 PlayerController 的 TeamID
        if (ACPlayerController* PC = GetController<ACPlayerController>())
        {
            PC->SetGenericTeamId(FGenericTeamId(NewTeamID));
        }
	}
	else
	{
		// 客户端情况下，通过 RPC 请求服务器修改
		Server_ChangeTeamID(NewTeamID);
	}
}

void ACCharacter::Server_ChangeTeamID_Implementation(uint8 NewTeamID)
{
	// 服务器拥有最终决定权
	// 所有客户端最终都会同步到
	SetGenericTeamId(FGenericTeamId(NewTeamID));

	// 2. 同步 PlayerController 的 TeamID
    if (ACPlayerController* PC = GetController<ACPlayerController>())
    {
        PC->SetGenericTeamId(FGenericTeamId(NewTeamID));
    }
}

bool ACCharacter::Server_ChangeTeamID_Validate(uint8 NewTeamID)
{
	// 你可以在这里做权限 / 范围检查
	// 比如：NewTeamID <= 2
	return true;
}

void ACCharacter::RefreshAllOverHeadUIForLocalPlayer()
{
	UWorld* World = GetWorld();  // this指针隐式传递
	if (!World)
	{
		return;
	}
    
    // 获取本地玩家控制器
    APlayerController* LocalPC = World->GetFirstPlayerController<APlayerController>();
    if (!LocalPC || !LocalPC->GetPawn())
    {
        return;
    }
    
    // 遍历场景中所有 Character
    for (TActorIterator<ACCharacter> It(World); It; ++It)
    {
    	ACCharacter* Character = *It;
    	if (Character && Character != this) // 排除自身避免无限递归
    	{
    		// 重新启动头顶UI
    		Character->SetStatusGaugeEnabled(true);
    	}
    }
}

void ACCharacter::OnRep_TeamID()
{
	// 团队ID更改的时候修改头顶血条
	SetOverHeadWidgetColor();
}

void ACCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent)
	{
		return;
	}

	if (bIsEnabled)
	{
		// 启用感知源
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	}
	else
	{
		// 禁用感知源
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem();
	}
}

// 新增辅助函数：尝试设置玩家名称
bool ACCharacter::TrySetPlayerName(UOverHeadStatsGauge* OverheadStatsGauge)
{
	if (!OverheadStatsGauge)
	{
		return false;
	}

	// 检查玩家名称是否已经设置
	if (OverheadStatsGauge->GetPlayerNameIsSet())
	{
		return true;
	}

	// 获取玩家状态来获取玩家名称
	// 首先尝试直接获取PlayerState（在某些情况下可能有效）
	if (APlayerState* MPlayerState = GetPlayerState<APlayerState>())
	{
		OverheadStatsGauge->SetPlayerNameFromPlayerState(MPlayerState);
		return true;
	}
	
	// 如果直接获取失败，尝试通过控制器获取PlayerState
	if (GetController())
	{
		if (APlayerState* MyPlayerState = GetController()->GetPlayerState<APlayerState>())
		{
			OverheadStatsGauge->SetPlayerNameFromPlayerState(MyPlayerState);
			return true;
		}
	}
	return false;
}
