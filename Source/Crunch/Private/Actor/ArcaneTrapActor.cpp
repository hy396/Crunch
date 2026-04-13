// 幻雨喜欢小猫咪


#include "ArcaneTrapActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"

AArcaneTrapActor::AArcaneTrapActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(RootComp);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 激活前不检测
	DetectionSphere->SetGenerateOverlapEvents(true);
	// 只和Pawn碰撞
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AArcaneTrapActor::InitializeTrap(
	FGenericTeamId InTeamId,
	FGameplayEffectSpecHandle InDamageSpec,
	FGameplayEffectSpecHandle InRootSpec,
	float InRadius,
	float InArmDelay,
	float InDuration)
{
	TeamId = InTeamId;
	DamageSpecHandle = InDamageSpec;
	RootSpecHandle = InRootSpec;

	// 设置检测半径
	DetectionSphere->SetSphereRadius(InRadius);

	// 绑定重叠事件
	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AArcaneTrapActor::OnOverlapBegin);

	// 延迟激活
	GetWorldTimerManager().SetTimer(ArmTimerHandle, this, &AArcaneTrapActor::ArmTrap, InArmDelay);

	// 设置存在时间
	GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &AArcaneTrapActor::OnDurationExpired, InDuration);
}

void AArcaneTrapActor::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void AArcaneTrapActor::ArmTrap()
{
	bArmed = true;
	// 开启碰撞检测
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 检查是否已有Actor在范围内
	TArray<AActor*> OverlappingActors;
	DetectionSphere->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
		{
			// 立即触发
			FHitResult DummyHit;
			OnOverlapBegin(DetectionSphere, Actor, nullptr, 0, false, DummyHit);
			return; // 触发后自毁
		}
	}
}

void AArcaneTrapActor::OnDurationExpired()
{
	Destroy();
}

void AArcaneTrapActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bArmed || !OtherActor)
	{
		return;
	}

	// 只对敌方生效
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!IsValid(TargetASC))
	{
		return;
	}

	// 服务器端应用效果
	if (HasAuthority())
	{
		// 应用伤害
		if (DamageSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data.Get());
		}

		// 应用定身
		if (RootSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*RootSpecHandle.Data.Get());
		}
	}

	// 触发后自毁
	// 关闭碰撞防止重复触发
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bArmed = false;

	// 延迟一点销毁（给特效播放时间）
	SetLifeSpan(0.5f);
}
