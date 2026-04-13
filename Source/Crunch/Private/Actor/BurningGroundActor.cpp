// 幻雨喜欢小猫咪


#include "BurningGroundActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

ABurningGroundActor::ABurningGroundActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(RootComp);
	// 只做查询用，不产生物理碰撞
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageSphere->SetGenerateOverlapEvents(true);
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABurningGroundActor::InitializeBurningGround(
	FGenericTeamId InTeamId,
	FGameplayEffectSpecHandle InBurnSpec,
	float InRadius,
	float InDuration,
	float InTickInterval)
{
	TeamId = InTeamId;
	BurnSpecHandle = InBurnSpec;

	DamageSphere->SetSphereRadius(InRadius);

	// 启动周期性伤害
	GetWorldTimerManager().SetTimer(TickTimerHandle, this, &ABurningGroundActor::DoBurnTick, InTickInterval, true, 0.f);

	// 设置存在时间
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &ABurningGroundActor::OnDurationExpired, InDuration);
}

void ABurningGroundActor::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ABurningGroundActor::DoBurnTick()
{
	if (!HasAuthority() || !BurnSpecHandle.IsValid())
	{
		return;
	}

	// 获取范围内所有Actor
	TArray<AActor*> OverlappingActors;
	DamageSphere->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor)
		{
			continue;
		}

		// 只对敌方生效
		if (GetTeamAttitudeTowards(*Actor) != ETeamAttitude::Hostile)
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
		if (IsValid(TargetASC))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*BurnSpecHandle.Data.Get());
		}
	}
}

void ABurningGroundActor::OnDurationExpired()
{
	GetWorldTimerManager().ClearTimer(TickTimerHandle);
	Destroy();
}
