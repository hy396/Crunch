// 幻雨喜欢小猫咪


#include "TargetActor_BlackHole.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"
// #include "Abilities/Tasks/AbilityTask_ApplyRootMotionRadialForce.h"
// #include "GameFramework/RootMotionSource.h"


// Sets default values
ATargetActor_BlackHole::ATargetActor_BlackHole()
{
	// 启用每帧Tick以维持黑洞持续效果（如引力场更新）
	PrimaryActorTick.bCanEverTick = true;
	// 启用网络复制确保客户端同步黑洞状态（位置/范围等）
	bReplicates = true;
	// 强制在服务器端生成目标数据（保证技能判定权威性）
	ShouldProduceTargetDataOnServer = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	// 创建球体组件，用于检测黑洞范围内的目标
	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Detection Sphere"));
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 绑定进入/离开黑洞范围的事件处理函数
	DetectionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATargetActor_BlackHole::ActorInBlackHoleRange);
	DetectionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ATargetActor_BlackHole::ActorLeftBlackHoleRange);

	// 黑洞本体特效
	VFXComponent = CreateDefaultSubobject<UParticleSystemComponent>("VFX Component");
	VFXComponent->SetupAttachment(GetRootComponent());

	// TODO:添加一点地面贴花特效
}

void ATargetActor_BlackHole::ConfigureBlackHole(float InBlackHoleRange, float InPullSpeed, float InTargetingInterval,
	float InBlackHoleDuration, const FGenericTeamId& InTeamId)
{
	PullSpeed = InPullSpeed;
	DetectionSphereComponent->SetSphereRadius(InBlackHoleRange);
	SetGenericTeamId(InTeamId);
	TargetingInterval = InTargetingInterval;
	BlackHoleDuration = InBlackHoleDuration;
	BlackHoleRange = InBlackHoleRange;
}

void ATargetActor_BlackHole::SetGenericTeamId(const FGenericTeamId& InTeamId)
{
	TeamId = InTeamId;
}

void ATargetActor_BlackHole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_BlackHole, TeamId);
	DOREPLIFETIME_CONDITION_NOTIFY(ATargetActor_BlackHole, BlackHoleRange, COND_None, REPNOTIFY_Always);
}

void ATargetActor_BlackHole::StartTargeting(class UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	UE_LOG(LogTemp, Warning, TEXT("Actor的位置%s"),*GetActorLocation().ToString())
	// 设置黑洞持续时间，时间结束后结束黑洞
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BlackHoleDurationTimerHandle, this, &ATargetActor_BlackHole::StopBlackHole, BlackHoleDuration);
	}
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(PeriodicalTargetingTimerHandle, this, &ATargetActor_BlackHole::DoTargetCheckAndReport, TargetingInterval, true);
	}
	// UAbilityTask_ApplyRootMotionRadialForce* Task = 
	// 		UAbilityTask_ApplyRootMotionRadialForce::ApplyRootMotionRadialForce(
	// 			OwningAbility, 
	// 			FName("RadialForce"), 
	// 			GetActorLocation(),          // 以技能释放者为中心
	// 			nullptr,                   // 无特定Actor位置
	// 			PullSpeed,                   // 强力击退
	// 			BlackHoleDuration,                     // 持续1秒
	// 			BlackHoleRange,                    // 500单位半径
	// 			false,                      // true推力模式，false为拉力
	// 			false,                     // 非叠加模式
	// 			false,                      // true忽略Z轴
	// 			nullptr,                   // 无距离衰减
	// 			nullptr,					// 使用预设时间曲线
	// 			false,                     // 使用动态方向
	// 			FRotator::ZeroRotator,     // 无固定方向
	// 			ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,  // 保持最终速度  ERootMotionFinishVelocityMode::Zero, // 结束时清零速度
	// 			FVector::ZeroVector,        // 无强制速度
	// 			600.f                         // 0为不限制速度，写别的数字可以设置速度上限
	// 	);
	//
	// if (Task)
	// {
	// 	Task->OnFinish.AddDynamic(this, &ATargetActor_BlackHole::StopBlackHole);
	// 	Task->ReadyForActivation();
	// }
}

// Called every frame
void ATargetActor_BlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 服务器端执行
	if (HasAuthority())
	{
		for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
		{
			AActor* Target = TargetPair.Key;
			UNiagaraComponent* NiagaraComponent = TargetPair.Value;
	
			FVector PullDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
			Target->SetActorLocation(Target->GetActorLocation() + PullDir * PullSpeed * DeltaTime);
			
			if (NiagaraComponent)
			{
				NiagaraComponent->SetVariablePosition(BlackHoleVFXOriginVariableName, VFXComponent->GetComponentLocation());
			}
			// UAbilityTask_ApplyRootMotionRadialForce* ApplyRootMotionRadialForce = 
		}
	}
	
}

void ATargetActor_BlackHole::ConfirmTargetingAndContinue()
{
	// TODO:暂时弃用
	// 结束黑洞
	// StopBlackHole();
	Super::ConfirmTargetingAndContinue();
}

void ATargetActor_BlackHole::CancelTargeting()
{
	// 结束黑洞
	StopBlackHole();
	Super::CancelTargeting();
}

void ATargetActor_BlackHole::BeginDestroy()
{
	if (GetWorld())
	{
		// 在去除Actor的时候停止定时器
		if (PeriodicalTargetingTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(PeriodicalTargetingTimerHandle);
		}
		if (BlackHoleDurationTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(BlackHoleDurationTimerHandle);
		}
	}
	Super::BeginDestroy();
}

void ATargetActor_BlackHole::ActorInBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryAddTarget(OtherActor);
}

void ATargetActor_BlackHole::ActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveTarget(OtherActor);
}

void ATargetActor_BlackHole::TryAddTarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget))
	{
		return;
	}
	// 只吸引敌方单位
	if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile)
	{
		return;
	}

	UNiagaraComponent* NiagaraComponent = nullptr;
	if (BlackHoleLinkVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BlackHoleLinkVFX, OtherTarget->GetRootComponent(), NAME_None,
			FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (NiagaraComponent)
		{
			// 设置特效组件的位置
			NiagaraComponent->SetVariablePosition(BlackHoleVFXOriginVariableName, VFXComponent->GetComponentLocation());
		}
	}
	ActorsInRangeMap.Add(OtherTarget, NiagaraComponent);
}

void ATargetActor_BlackHole::RemoveTarget(AActor* OtherTarget)
{
	if (!OtherTarget)
	{
		return;
	}
	UNiagaraComponent* VFXComp;
	if (ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VFXComp))
	{
		if (VFXComp)
		{
			// 销毁特效组件
			VFXComp->DestroyComponent();
		}
	}
}

void ATargetActor_BlackHole::StopBlackHole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	// 直接销毁所有Niagara组件
	for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
	{
		FinalTargets.Add(TargetPair.Key);
		UNiagaraComponent* NiagaraComponent = TargetPair.Value;
		if (IsValid(NiagaraComponent))
		{
			NiagaraComponent->DestroyComponent();
		}
	}

	// 清空映射避免残留无效引用
	ActorsInRangeMap.Empty();

	// 构造目标数据（所有被吸引目标 + 黑洞中心点）
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	// 目标对象数组
	FGameplayAbilityTargetData_ActorArray* TargetActorArray = new FGameplayAbilityTargetData_ActorArray;
	TargetActorArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetActorArray);
	// 黑洞中心点
	FGameplayAbilityTargetData_SingleTargetHit* BlowupLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	BlowupLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(BlowupLocation);

	// TODO:由于我修改为持续模式，因此用取消来结束能力
	CanceledDelegate.Broadcast(TargetDataHandle);
	// TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_BlackHole::DoTargetCheckAndReport()
{
	// 服务器中执行
	if (!HasAuthority()) return;
	
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	// 直接销毁所有Niagara组件
	for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap)
	{
		FinalTargets.Add(TargetPair.Key);
	}
	// 构造目标数据（所有被吸引目标）
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	// 目标对象数组
	FGameplayAbilityTargetData_ActorArray* TargetActorArray = new FGameplayAbilityTargetData_ActorArray;
	TargetActorArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetActorArray);

	// 广播目标数据就绪事件
	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_BlackHole::OnRep_BlackHoleRange()
{
	// 更新球体组件的半径
	DetectionSphereComponent->SetSphereRadius(BlackHoleRange);
}

