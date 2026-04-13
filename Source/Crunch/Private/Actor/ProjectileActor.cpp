// 幻雨喜欢小猫咪


#include "ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AProjectileActor::AProjectileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 创建根组件
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>("Root Comp");
	SetRootComponent(RootComp);

	// 启用网络同步
	bReplicates = true;
}

void AProjectileActor::ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget,
		FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle, FGenericDamageEffectDef InDamageEffectDef)
{
	// 设置目标以及速度
	Target = InTarget;
	ProjectileSpeed = InSpeed;
	// 方向
	FRotator OwnerViewRot = GetActorRotation();
	// 队伍设置
	SetGenericTeamId(InTeamId);

	// 获取拥有者视角（如有需要可用于弹道修正）
	if (bCorrectDirectionByCamera && GetOwner())
	{
		FVector OwnerViewLoc;
		GetOwner()->GetActorEyesViewPoint(OwnerViewLoc, OwnerViewRot);
	}
	
	// 设置移动方向
	MoveDir = OwnerViewRot.Vector();
	HitEffectSpecHandle = InHitEffectHandle;

	if (InDamageEffectDef.DamageEffect)
	{
		// 伤害效果定义
		DamageEffectDef = InDamageEffectDef;
	}
	
	// 设置最大飞行时间，到达后自动销毁
	float TravelMaxTime = InMaxDistance / InSpeed;
	GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AProjectileActor::TravelMaxDistanceReached, TravelMaxTime);
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileActor, MoveDir);
	DOREPLIFETIME(AProjectileActor, TeamId);
	DOREPLIFETIME(AProjectileActor, ProjectileSpeed);
}

void AProjectileActor::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void AProjectileActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// 忽略自身和拥有者
	if (!OtherActor || OtherActor == GetOwner())
		return;

	// 只对敌方目标生效
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
		return;

	// 获取目标的能力系统组件
	UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (IsValid(OtherASC))
	{
		// // 👇 新增: 检查目标是否死亡
		// if (OtherASC->HasMatchingGameplayTag(TGameplayTags::Stats_Dead))
        // {
        //     return; // 跳过死亡目标
        // }
		// 服务器应用命中效果
		if (HasAuthority() && HitEffectSpecHandle.IsValid())
		{
			for (const auto& TypePair : DamageEffectDef.DamageTypeDefinitions)
			{
				// 创建效果Spec句柄，指定效果类、能力等级和上下文
				FGameplayEffectSpecHandle EffectSpecHandle = HitEffectSpecHandle;
				float TotalModifier = TypePair.Value.BaseDamage.GetValueAtLevel(HitEffectSpecHandle.Data->GetLevel());
				for (const auto& Modifier : TypePair.Value.AttributeDamageModifiers)
				{
					UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Modifier.Key, Modifier.Value);
				}
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, TypePair.Key, TotalModifier);
				// 在目标上应用游戏效果规范
				OtherASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
			}
			// OtherASC->ApplyGameplayEffectSpecToSelf(*HitEffectSpecHandle.Data.Get());
			if (bAutoDestroyOnHit)
			{
				GetWorldTimerManager().ClearTimer(ShootTimerHandle);
			}
		}

		// 构造命中结果
		FHitResult HitResult;
		HitResult.ImpactPoint = GetActorLocation();
		HitResult.ImpactNormal = GetActorForwardVector();
		
		// 发送本地GameplayCue（用于特效、音效等）
		SendLocalGameplayCue(OtherActor, HitResult);

		// TODO:不销毁的话可以做成子弹对象池
		// 销毁投射物
		if (bAutoDestroyOnHit)
		{
			Destroy();
		}
	}
}

void AProjectileActor::SetHitGameplayCueTag(const FGameplayTag InGameplayTag)
{
	HitGameplayCueTag = InGameplayTag;
}

void AProjectileActor::TravelMaxDistanceReached()
{
	// if (HasAuthority())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("达到最大"));
	// 	// 构造命中结果
	// 	FHitResult HitResult;
	// 	HitResult.ImpactPoint = GetActorLocation();
	// 	HitResult.ImpactNormal = GetActorForwardVector();
	// 	
	// 	// 发送本地GameplayCue（用于特效、音效等）
	// 	SendLocalGameplayCue(GetOwner(), HitResult);
	// }
	

	Destroy();
}

void AProjectileActor::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, HitGameplayCueTag, EGameplayCueEvent::Executed, CueParams);
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 服务器端：如果有目标则实时调整方向
	if (HasAuthority())
	{
		if (Target.IsValid())
		{
			MoveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
	}

	// 按当前方向和速度移动
	SetActorLocation(GetActorLocation() + MoveDir * ProjectileSpeed * DeltaTime);
}

