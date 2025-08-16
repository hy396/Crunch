// 幻雨喜欢小猫咪


#include "TargetActor_Around.h"

#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATargetActor_Around::ATargetActor_Around()
{
	PrimaryActorTick.bCanEverTick = true;
	// 网络设置：在服务器和客户端同步
	bReplicates = true;
	// 重要：确保服务器能产生目标数据
	ShouldProduceTargetDataOnServer = true;
	// 创建根组件
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	// 创建球形碰撞体用于范围检测
	DetectionSphere = CreateDefaultSubobject<USphereComponent>("Detection Sphere");
	DetectionSphere->SetupAttachment(GetRootComponent());

	// 配置碰撞设置：只检测Pawn类型的重叠
	DetectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// 绑定重叠开始事件回调
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATargetActor_Around::ActorInDetectionRange);

}

void ATargetActor_Around::ConfigureDetection(float DetectionRadius, const FGenericTeamId& InTeamId,
	const FGameplayTag& InLocalGameplayCueTag)
{
	// 设置队伍关系
	SetGenericTeamId(InTeamId);
	// 更新碰撞体大小
	DetectionSphere->SetSphereRadius(DetectionRadius);
	// 同步到网络变量
	TargetDetectionRadius = DetectionRadius;
	// 设置视觉提示标签
	LocalGameplayCueTag = InLocalGameplayCueTag;
}

void ATargetActor_Around::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ATargetActor_Around::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 注册需要网络同步的属性
	DOREPLIFETIME(ATargetActor_Around, TeamId);					// 队伍ID
	DOREPLIFETIME(ATargetActor_Around, LocalGameplayCueTag);	// 视觉标签
	DOREPLIFETIME(ATargetActor_Around, TargetDetectionRadius);	// 检测半径
}

void ATargetActor_Around::OnRep_TargetDetectionRadiusReplicated()
{
	// 客户端收到半径更新时，同步更新碰撞体大小
	DetectionSphere->SetSphereRadius(TargetDetectionRadius);
}

void ATargetActor_Around::ActorInDetectionRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 忽略无效Actor
	if (!OtherActor) return;

	// 获取能力拥有者（避免检测到自己）
	AActor* AvatarActor = nullptr;
	if (OwningAbility)
	{
		AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	}

	// 忽略自身和拥有者
	if (OtherActor == AvatarActor) return;
	if (OtherActor == this) return;

	// 队伍关系检查：只处理敌对目标
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile) return;

	// 服务器处理目标数据
	if (HasAuthority())
	{
		// 构建目标数据
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		FGameplayAbilityTargetData_ActorArray* ActorArray = new FGameplayAbilityTargetData_ActorArray;
		ActorArray->SetActors(TArray<TWeakObjectPtr<AActor>>{OtherActor});
		TargetDataHandle.Add(ActorArray);
		
		// 通知能力系统目标已就绪
		TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	}
	
	// 播放GC特效
	FHitResult HitResult;
	HitResult.ImpactPoint = OtherActor->GetActorLocation();  // 命中点为目标位置
	HitResult.ImpactNormal = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal(); // 命中方向
	UCAbilitySystemStatics::SendLocalGameplayCue(OtherActor, HitResult, LocalGameplayCueTag);
}

