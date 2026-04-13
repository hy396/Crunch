// 幻雨喜欢小猫咪


#include "ArcaneOrbProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AArcaneOrbProjectile::AArcaneOrbProjectile()
{
	// 穿透：命中后不销毁
	// 注意：bAutoDestroyOnHit 是父类私有的，但构造函数中默认值可在蓝图覆盖
	// 这里我们通过重写 NotifyActorBeginOverlap 来实现穿透逻辑
}

void AArcaneOrbProjectile::Tick(float DeltaTime)
{
	// 记录起始位置（第一帧）
	if (StartLocation.IsZero())
	{
		StartLocation = GetActorLocation();
	}

	// 调用父类Tick（处理移动）
	Super::Tick(DeltaTime);

	// 更新飞行距离
	TraveledDistance = FVector::Dist(StartLocation, GetActorLocation());
}

float AArcaneOrbProjectile::GetDistanceRatio() const
{
	if (MaxDistance <= 0.f)
	{
		return 0.f;
	}
	return FMath::Clamp(TraveledDistance / MaxDistance, 0.f, 1.f);
}

void AArcaneOrbProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// 忽略自身和拥有者
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	// 只对敌方生效
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
	{
		return;
	}

	// 检查是否已经命中过
	for (const TWeakObjectPtr<AActor>& HitActor : AlreadyHitActors)
	{
		if (HitActor.Get() == OtherActor)
		{
			return; // 已经命中过，跳过
		}
	}

	// 记录已命中
	AlreadyHitActors.Add(OtherActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!IsValid(TargetASC))
	{
		return;
	}

	// 服务器端应用伤害（距离越远伤害越高由GE的Level缩放实现）
	// 这里我们不调用父类的NotifyActorBeginOverlap（因为父类会销毁Actor）
	// 而是直接应用伤害

	if (HasAuthority())
	{
		// 回复施法者法力
		if (ManaRestoreEffectHandle.IsValid())
		{
			UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
			if (IsValid(OwnerASC))
			{
				OwnerASC->ApplyGameplayEffectSpecToSelf(*ManaRestoreEffectHandle.Data.Get());
			}
		}
	}

	// 注意：实际伤害由父类ProjectileActor的DamageEffectDef机制处理
	// 因为我们没有调用Super::NotifyActorBeginOverlap，需要手动处理
	// 但为了保持一致性，我们把穿透弹的bAutoDestroyOnHit设为false
	// 然后让父类处理伤害但不销毁
	// 因此直接调用父类——父类会检查bAutoDestroyOnHit来决定是否销毁
	Super::NotifyActorBeginOverlap(OtherActor);
}
