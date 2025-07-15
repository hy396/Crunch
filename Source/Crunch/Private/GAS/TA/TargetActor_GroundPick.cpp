// 幻雨喜欢小猫咪


#include "GAS/TA/TargetActor_GroundPick.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Crunch/Crunch.h"
#include "Engine/OverlapResult.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;
	// 创建根组件
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Comp"));

	// 创建贴花组件用于显示范围
	DecalComp = CreateDefaultSubobject<UDecalComponent>("Decal Comp");
	DecalComp->SetupAttachment(GetRootComponent());
}

void ATargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
	TargetAreaRadius = NewRadius;
	// 同步贴花的显示大小
	DecalComp->DecalSize = FVector{NewRadius};
}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	// 检测目标
	TArray<FOverlapResult> OverlapResults;
	
	// 设置碰撞查询参数，仅检测Pawn
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	// 创建球形碰撞球体，设置半径和位置
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(TargetAreaRadius);

	// 检测碰撞
	GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape
		);

	TSet<AActor*> TargetActors;

	// 获取能力使用者的团队接口
	IGenericTeamAgentInterface* OwnerTeamInterface = nullptr;
	if (OwningAbility)
	{
		OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
	}

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		// 检测到友军，友军为false，不打友军跳过
		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Friendly && !bShouldTargetFriendly)
			continue;
		// 检测到敌军，敌军为false，不打敌军跳过
		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Hostile && !bShouldTargetEnemy)
			continue;

		// 添加目标
		TargetActors.Add(OverlapResult.GetActor());
	}

	// 创建目标数据
	FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(), false);

	// 添加命中点信息（用于特效等）
	FGameplayAbilityTargetData_SingleTargetHit* HitLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	HitLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetData.Add(HitLocation);
	// 触发目标数据已就绪委托
	TargetDataReadyDelegate.Broadcast(TargetData);
}

void ATargetActor_GroundPick::SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy)
{
	bShouldTargetFriendly = bTargetFriendly;
	bShouldTargetEnemy = bTargetEnemy;
}

void ATargetActor_GroundPick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 检测是否是本地玩家控制(只在本地客户端中显示)
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		// 设置目标点位置
		SetActorLocation(GetTargetPoint());
	}
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController())
		return GetActorLocation();

	FHitResult TraceResult;

	FVector ViewLoc;
	FRotator ViewRot;
	
	// 获取视角位置和朝向
	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);

	// 获取射线终点位置
	FVector TraceEnd = ViewLoc + ViewRot.Vector() * TargetTraceRange;

	// 射线检测
	GetWorld()->LineTraceSingleByChannel(
		TraceResult,
		ViewLoc,
		TraceEnd,
		ECC_TARGET);

	// 如果没有命中，向下做一次射线检测，把结果点放地上
	if (!TraceResult.bBlockingHit)
	{
		GetWorld()->LineTraceSingleByChannel(TraceResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_TARGET);
	}

	// 如果依然没有命中，返回当前位置
	if (!TraceResult.bBlockingHit)
	{
		return GetActorLocation();
	}

	// 绘制调试球体
	if (bShouldDrawDebug)
	{
		DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, TargetAreaRadius, 32, FColor::Red);
	}

	return TraceResult.ImpactPoint;
}
