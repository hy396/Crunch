// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "ProjectileActor.generated.h"

/**
 * 投射物Actor类
 * 负责处理子弹/弹道的移动、命中、效果应用等逻辑
 */
UCLASS()
class CRUNCH_API AProjectileActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileActor();

	// 发射子弹，初始化速度、距离、目标、队伍ID和命中效果
	void ShootProjectile(
		float InSpeed,
		float InMaxDistance,
		const AActor* InTarget,
		FGenericTeamId InTeamId,
		FGameplayEffectSpecHandle InHitEffectHandle,
		FGenericDamageEffectDef InDamageEffectDef
	);

	// 网络属性同步
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 设置队伍ID
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	// 获取队伍ID
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	// 处理与其他Actor的重叠（命中检测）
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable)
	void SetHitGameplayCueTag(FGameplayTag InGameplayTag);
	// 创建爆炸效果
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnExplosion();
private:
	// 命中时触发的GameplayCue标签
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cue", meta = (Categories = "Projectile"))
	FGameplayTag HitGameplayCueTag;
	
	// 伤害效果定义
	UPROPERTY(EditDefaultsOnly, Category = "DamageEffect")
	FGenericDamageEffectDef DamageEffectDef;

	// 是否根据摄像机方向修正弹道方向
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (DisplayName = "是否根据摄像机方向修正弹道方向"))
	bool bCorrectDirectionByCamera = true;

	// 是否碰撞后自动销毁
	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (DisplayName = "是否碰撞后自动销毁"))
	bool bAutoDestroyOnHit = true;
	
	// 投射物所属队伍ID（同步属性）
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	// 投射物移动方向（同步属性）
	UPROPERTY(Replicated)
	FVector MoveDir;

	// 投射物速度（同步属性）
	UPROPERTY(Replicated)
	float ProjectileSpeed;

	// 当前目标Actor（使用弱指针，防止目标销毁后悬空指针崩溃）
	TWeakObjectPtr<const AActor> Target;

	// 命中时应用的效果句柄
	FGameplayEffectSpecHandle HitEffectSpecHandle;

	// 发射定时器句柄
	FTimerHandle ShootTimerHandle;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	// 达到最大射程时的处理
	void TravelMaxDistanceReached();

	// 发送本地GameplayCue（用于特效、音效等）
	void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult);
};
