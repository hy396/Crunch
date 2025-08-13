// 幻雨喜欢小猫咪


#include "TargetActor_Line.h"

#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Crunch/Crunch.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATargetActor_Line::ATargetActor_Line()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;

	AvatarActor = nullptr;

	// 创建根组件
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	// 创建目标检测球体组件（用于检测终点附近的目标）
	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>("Target End Detection Sphere");
	TargetEndDetectionSphere->SetupAttachment(GetRootComponent());
	TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_SPRING_ARM, ECR_Ignore);

	// 创建激光特效组件
	// LaserVFX = CreateDefaultSubobject<UNiagaraComponent>("LaserVFX");
	// LaserVFX->SetupAttachment(GetRootComponent());

	// 新添加的
	LaserFX = CreateDefaultSubobject<UParticleSystemComponent>("Laser FX");
	LaserFX->SetupAttachment(GetRootComponent());
}

void ATargetActor_Line::ConfigureTargetSetting(float NewTargetRange, float NewDetectionCylinderRadius,
	float NewTargetingInterval, FGenericTeamId OwnerTeamId, bool bShouldDrawDebug)
{
	TargetRange = NewTargetRange;
	DetectionCylinderRadius = NewDetectionCylinderRadius;
	TargetingInterval = NewTargetingInterval;
	SetGenericTeamId(OwnerTeamId);
	bDrawDebug = bShouldDrawDebug;
}

void ATargetActor_Line::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ATargetActor_Line::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_Line, TeamId);
	DOREPLIFETIME(ATargetActor_Line, TargetRange);
	DOREPLIFETIME(ATargetActor_Line, DetectionCylinderRadius);
	DOREPLIFETIME(ATargetActor_Line, AvatarActor);
}

void ATargetActor_Line::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	if (!OwningAbility)
		return;

	AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	// 在服务器上开启定时器周期检测目标
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(PeriodicalTargetingTimerHandle, this, &ATargetActor_Line::DoTargetCheckAndReport, TargetingInterval, true);
	}
}

void ATargetActor_Line::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 更新激光
	UpdateTargetTrace();
}

void ATargetActor_Line::BeginDestroy()
{
	// 在去除Actor的时候停止定时器
	if (GetWorld() && PeriodicalTargetingTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(PeriodicalTargetingTimerHandle);
	}
	Super::BeginDestroy();
}

void ATargetActor_Line::DoTargetCheckAndReport()
{
	// 服务器中执行
	if (!HasAuthority()) return;

	// 获取重叠的Actor
	TArray<AActor*> OverlappingActorSet;
	TargetEndDetectionSphere->GetOverlappingActors(OverlappingActorSet);

	// 筛选有效目标
	TArray<TWeakObjectPtr<AActor>> OverlappingActors;
	for (AActor* Actor : OverlappingActorSet)
	{
		if (ShouldReportActorAsTarget(Actor))
		{
			OverlappingActors.Add(Actor);
		}
	}

	// 构建目标数据
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray;
	ActorArray->SetActors(OverlappingActors);
	TargetDataHandle.Add(ActorArray);

	// 广播目标数据就绪事件
	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_Line::UpdateTargetTrace()
{
	// 设置激光起始点
	FVector ViewLocation = GetActorLocation();
	FRotator ViewRotation = GetActorRotation();
	if (AvatarActor)
	{
		// 获取摄像机位置和瞄准的旋转
		AvatarActor->GetActorEyesViewPoint(ViewLocation, ViewRotation);
	}

	// 计算激光方向和终点
	FVector LookEndPoint = ViewLocation + ViewRotation.Vector() * 100000;
	// 根据角色位置以及相机方向计算出激光方向
	FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookEndPoint);
	SetActorRotation(LookRotation);

	// 获取激光终点 ： 由激光发射位置，由角色朝向，和激光长度设置终点位置
	FVector SweepEndLocation = GetActorLocation() + LookRotation.Vector() * TargetRange;


	TArray<FHitResult> HitResults;

	// 配置碰撞参数，忽略自身和拥有者
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.AddIgnoredActor(this);
	FCollisionResponseParams CollisionResponseParams(ECR_Overlap);
	// 球形检测，查找路径上的目标
	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), SweepEndLocation, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(DetectionCylinderRadius), QueryParams, CollisionResponseParams);

	FVector LineEndLocation = SweepEndLocation;
	float LineLength = TargetRange;
	// 命中第一个非友方目标则截断激光
	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor())
		{
			if (GetTeamAttitudeTowards(*HitResult.GetActor()) != ETeamAttitude::Friendly)
			{
				LineEndLocation = HitResult.ImpactPoint;
				LineLength = FVector::Distance(GetActorLocation() , LineEndLocation);
				break;
			}
		}
	}
	
	// 更新终点检测球体和激光长度
	TargetEndDetectionSphere->SetWorldLocation(LineEndLocation);
	// if (LaserVFX)
	// {
	// 	LaserVFX->SetVariableFloat(LaserVFXLengthParamName, LineLength/100.f);
	// }

	if (LaserFX)
	{
		LaserFX->SetVectorParameter(LaserFXBeamEndParamName, FVector(LineLength - 5.f, 0.f,0.f));
		LaserFX->SetVectorParameter(LaserFXLengthParamName, FVector(1.f, 1.f,LineLength/20.f));
		LaserFX->SetFloatParameter(LaserFXtileVcorrectParamName, LineLength/500.f);
	}

	// 要删除的
	// 修改begin
	//
	// // 设置检测对象类型为Pawn
	// TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 只检测Pawn类型
	// TArray<AActor*> IgnoredActors; // 忽略的Actor
	//
	// EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
	//
	// // 球形多重检测，查找路径上的所有目标
	// UKismetSystemLibrary::SphereTraceMultiForObjects(
	// 	this, GetActorLocation(), SweepEndLocation, DetectionCylinderRadius, 
	// 	ObjectTypes, false, IgnoredActors, DrawDebugTrace, HitResults, false);
	// 修改ENd
}

bool ATargetActor_Line::ShouldReportActorAsTarget(const AActor* ActorToCheck) const
{
	// 基本无效检查
	if (!ActorToCheck) return false;
	if (ActorToCheck == AvatarActor) return false; // 忽略自身
	if (ActorToCheck == this) return false; // 忽略自身

	// 团队关系检查（只报告敌对目标）
	if (GetTeamAttitudeTowards(*ActorToCheck) != ETeamAttitude::Hostile)
		return false;

	return true;
}



