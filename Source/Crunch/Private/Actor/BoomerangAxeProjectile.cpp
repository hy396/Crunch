// 幻雨喜欢小猫咪


#include "BoomerangAxeProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"

ABoomerangAxeProjectile::ABoomerangAxeProjectile()
{
	// 回旋斧不在第一次命中时销毁
	// bAutoDestroyOnHit在父类是EditDefaultsOnly，需要在蓝图中设为false
	// 或者我们在NotifyActorBeginOverlap中重写逻辑
}

void ABoomerangAxeProjectile::Tick(float DeltaTime)
{
	if (bIsReturning && HasAuthority())
	{
		// 回旋阶段：朝Owner飞行
		AActor* OwnerActor = GetOwner();
		if (OwnerActor)
		{
			FVector ToOwner = (OwnerActor->GetActorLocation() - GetActorLocation());
			float DistToOwner = ToOwner.Size();

			// 检查是否回到Owner身边
			if (DistToOwner <= ReturnCaptureRadius)
			{
				// 通知Owner技能：斧头已返回
				if (ReturnEventTag.IsValid())
				{
					FGameplayEventData EventData;
					EventData.Instigator = this;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, ReturnEventTag, EventData);
				}

				Destroy();
				return;
			}
		}
	}

	// 父类处理移动逻辑
	Super::Tick(DeltaTime);
}

void ABoomerangAxeProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// 忽略Owner
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	// 只对敌方生效
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
	{
		return;
	}

	// 检查是否已命中过
	if (HasAlreadyHit(OtherActor))
	{
		return;
	}

	// 记录命中
	RecordHit(OtherActor);

	// 调用父类的重叠处理（应用伤害，但不销毁——蓝图中bAutoDestroyOnHit=false）
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ABoomerangAxeProjectile::StartReturn()
{
	bIsReturning = true;

	// 清除最大距离定时器（父类设的自毁定时器）
	// 由Tick中的距离检查来管理生命周期
}

bool ABoomerangAxeProjectile::HasAlreadyHit(AActor* Actor) const
{
	const TArray<TWeakObjectPtr<AActor>>& HitList = bIsReturning ? ReturnHitActors : OutwardHitActors;
	for (const TWeakObjectPtr<AActor>& HitActor : HitList)
	{
		if (HitActor.Get() == Actor)
		{
			return true;
		}
	}
	return false;
}

void ABoomerangAxeProjectile::RecordHit(AActor* Actor)
{
	if (bIsReturning)
	{
		ReturnHitActors.Add(Actor);
	}
	else
	{
		OutwardHitActors.Add(Actor);
	}
}
