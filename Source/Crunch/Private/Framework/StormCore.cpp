// 幻雨喜欢小猫咪


#include "StormCore.h"

#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AStormCore::AStormCore()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 创建影响力范围检测组件
	InfluenceRange = CreateDefaultSubobject<USphereComponent>(TEXT("Influence Range"));
	InfluenceRange->SetupAttachment(GetRootComponent());
	InfluenceRange->SetSphereRadius(InfluenceRadius);

	// 绑定重叠事件
	InfluenceRange->OnComponentBeginOverlap.AddDynamic(this, &AStormCore::NewInfluencerInRange);
	InfluenceRange->OnComponentEndOverlap.AddDynamic(this, &AStormCore::InfluencerLeftRange);

	// 创建观察相机
	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(GetRootComponent());

	// 创建地面贴花
	GroundDecalComponent = CreateDefaultSubobject<UDecalComponent>("Ground Decal Component");
	GroundDecalComponent->SetupAttachment(GetRootComponent());
	GroundDecalComponent->DecalSize = FVector{200.f, InfluenceRadius, InfluenceRadius};
}

void AStormCore::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 同步CoreToCapture属性，总是通知
	DOREPLIFETIME_CONDITION_NOTIFY(AStormCore, CoreToCapture, COND_None, REPNOTIFY_Always);
}

float AStormCore::GetProgress() const
{
	// 获取队伍2目标点位置
	FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();
	// 计算当前核心到队伍2目标点的距离（忽略Z轴）
	FVector VectorFromTeamOne = GetActorLocation() - TeamTwoGoalLoc;
	VectorFromTeamOne.Z = 0.f; // 忽略Z轴差异

	// 计算当前位置占总长度的比例
	return VectorFromTeamOne.Length() / TravelLength;
}

// Called when the game starts or when spawned
void AStormCore::BeginPlay()
{
	Super::BeginPlay();
	if (TeamOneGoal && TeamTwoGoal)
	{
		// 计算两个目标点之间的距离
		FVector TeamOneGoalLoc = TeamOneGoal->GetActorLocation();
		FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();

		FVector GoalOffset = TeamOneGoalLoc - TeamTwoGoalLoc;
		GoalOffset.Z = 0; // 仅考虑水平距离
		TravelLength = GoalOffset.Length(); // 保存总长度用于进度计算
	}
}

void AStormCore::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	OwnerAIC = Cast<AAIController>(NewController); // 保存AI控制器引用
}

// Called every frame
void AStormCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 如果正在捕获核心，将其向风暴核心移动
	if (CoreToCapture)
	{
		FVector CoreMoveDir = (GetMesh()->GetComponentLocation() - CoreToCapture->GetActorLocation()).GetSafeNormal();
		CoreToCapture->AddActorWorldOffset(CoreMoveDir * CoreCaptureSpeed * DeltaTime);
	}
}

// Called to bind functionality to input
void AStormCore::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AStormCore::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	// 获取属性名称
	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	
	// 当影响力半径改变时，更新组件
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AStormCore, InfluenceRadius))
	{
		InfluenceRange->SetSphereRadius(InfluenceRadius); // 更新球体半径
		FVector DecalSize = GroundDecalComponent->DecalSize;
		GroundDecalComponent->DecalSize = FVector{DecalSize.X, InfluenceRadius, InfluenceRadius}; // 更新贴花大小
	}
}

void AStormCore::NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 检查是否到达目标点
	if (OtherActor == TeamOneGoal)
	{
		GoalReached(0); // 队伍一胜利
		return;
	}

	if (OtherActor == TeamTwoGoal)
	{
		GoalReached(1); // 队伍二胜利
		return;
	}
	// 获取走进范围的角色的队伍ID
	IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamInterface)
	{
		FGenericTeamId TeamId = OtherTeamInterface->GetGenericTeamId();

		// 更新对应队伍的计数器
		if (TeamId.GetId() == 0) TeamOneInfluencerCount++;
		else if (TeamId.GetId() == 1) TeamTwoInfluencerCount++;
		UpdateTeamWeight(); // 重新计算权重
	}
}

void AStormCore::InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 获取走出范围的角色的队伍ID
	IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (OtherTeamInterface)
	{
		FGenericTeamId TeamId = OtherTeamInterface->GetGenericTeamId();

		// 更新对应队伍的计数器（确保不小于0）
		if (TeamId.GetId() == 0) TeamOneInfluencerCount = FMath::Max(0, TeamOneInfluencerCount - 1);
		else if (TeamId.GetId() == 1) TeamTwoInfluencerCount = FMath::Max(0, TeamTwoInfluencerCount - 1);
		
		UpdateTeamWeight(); // 重新计算权重
	}
}

void AStormCore::UpdateTeamWeight()
{
	OnTeamInfluenceCountUpdated.Broadcast(TeamOneInfluencerCount, TeamTwoInfluencerCount);
	if (TeamOneInfluencerCount == TeamTwoInfluencerCount)
	{
		// 平衡状态
		TeamWeight = 0.f;
	}else
	{
		// 计算公式：（队伍1的数量 - 队伍2的数量）/ 总数
		float TeamOffset = TeamOneInfluencerCount - TeamTwoInfluencerCount;
		float TeamTotal = TeamOneInfluencerCount + TeamTwoInfluencerCount;
		TeamWeight = TeamOffset / TeamTotal;
	}
	
	UpdateGoal(); // 根据新权重更新目标
	UE_LOG(LogTemp, Warning, TEXT("队伍一的数量: %d, 队伍二的数量: %d, Weight: %f"), TeamOneInfluencerCount, TeamTwoInfluencerCount, TeamWeight);
}

void AStormCore::UpdateGoal()
{
	if (!HasAuthority()) return; // 仅在服务器执行
	if (!OwnerAIC) return; // 需要有效的AI控制器

	// 根据权重选择目标点
	if (TeamWeight > 0) 
	{
		OwnerAIC->MoveToActor(TeamOneGoal); // 向队伍1的目标移动
		UE_LOG(LogTemp, Warning, TEXT("向队伍1的目标移动"))
	}
	else 
	{
		OwnerAIC->MoveToActor(TeamTwoGoal); // 向队伍2的目标移动
		UE_LOG(LogTemp, Warning, TEXT("向队伍2的目标移动"))
	}

	// 根据权重绝对值调整移动速度
	float Speed = MaxMoveSpeed * FMath::Abs(TeamWeight);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	UE_LOG(LogTemp, Warning, TEXT("移动速度为: %f"), Speed);
}

void AStormCore::OnRep_CoreToCapture()
{
	if (CoreToCapture)
	{
		CaptureCore(); // 开始捕获核心
	}
}

void AStormCore::GoalReached(int WiningTeam)
{
	// 广播胜利事件
	OnGoalReachedDelegate.Broadcast(this, WiningTeam);
	
	if (!HasAuthority()) return; // 仅在服务器执行后续逻辑
	
	MaxMoveSpeed = 0.f; // 停止移动
	
	// 确定要捕获的核心（失败方的核心）
	CoreToCapture = (WiningTeam == 0) ? TeamTwoCore : TeamOneCore;
	CaptureCore(); // 开始捕获流程
}

void AStormCore::CaptureCore()
{
	// 播放扩张动画
	float ExpandDuration = GetMesh()->GetAnimInstance()->Montage_Play(ExpandMontage);
	
	// 计算捕获速度（距离/时间）
	CoreCaptureSpeed = FVector::Distance(GetMesh()->GetComponentLocation(), CoreToCapture->GetActorLocation()) / ExpandDuration;
	
	CoreToCapture->SetActorEnableCollision(false); // 禁用碰撞
	GetCharacterMovement()->MaxWalkSpeed = 0.f; // 停止移动
	
	// 设置定时器：动画完成后执行回调进行捕获核心
	FTimerHandle ExpandTimerHandle;
	GetWorldTimerManager().SetTimer(ExpandTimerHandle, this, &AStormCore::ExpandFinished, ExpandDuration);
}

void AStormCore::ExpandFinished()
{
	// 将核心移动到风暴中心
	CoreToCapture->SetActorLocation(GetMesh()->GetComponentLocation());
	CoreToCapture->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "root");
	
	// 播放捕获动画
	GetMesh()->GetAnimInstance()->Montage_Play(CaptureMontage);
}

