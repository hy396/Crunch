// 幻雨喜欢小猫咪


#include "Crunch/Private/Character/CCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Gameplay/OverHeadStatsGauge.h"

ACCharacter::ACCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// 禁用网格的碰撞功能
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>(TEXT("CAbilitySystemComponent"));
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("CAttributeSet"));
	//CAbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制,经过我的测试本来就是true
	
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComponent"));
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());
}

void ACCharacter::ServerSideInit()
{
	// 设置当前角色作为Owner和Avatar，用于后续的能力和效果应用
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	CAbilitySystemComponent->ApplyInitialEffects();
	CAbilitySystemComponent->GiveInitialAbilities();
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

void ACCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatusWidget();
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

void ACCharacter::ConfigureOverHeadStatusWidget()
{
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
	UOverHeadStatsGauge* OverheadStatsGuage = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatsGuage)
	{
		// 使用能力系统组件配置头顶统计量表
		OverheadStatsGuage->ConfigureWithASC(GetAbilitySystemComponent());
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
	
}

