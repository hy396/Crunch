// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Minion.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Actor.h"
#include "MinionBarrack.generated.h"

// class APlayerStart;

/**
 * 小兵兵营类，负责批量生成和管理小兵
 * 支持队伍分配、目标设置、定时批量生成等功能
 */
UCLASS()
class AMinionBarrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinionBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// 游戏结束时停止生成小兵
    void StopSpawning();

private:
	// 兵营所属队伍ID
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "队伍ID"))
	FGenericTeamId BarrackTeamId;

	// 每组小兵生成的数量
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "每组小兵生成的数量"))
	int32 MinionPerGroup = 5;

	// 兵营的生成间隔
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "小兵生成间隔"))
	float GroupSpawnInterval = 15.f;
	
	// 小兵对象池
	UPROPERTY()
	TArray<TObjectPtr<AMinion>> MinionPool;

	// 小兵的目标点（如推进目标）
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "小兵的目标点"))
	TObjectPtr<AActor> Goal;

	// 小兵的类（用于生成小兵实例）
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "小兵的类"))
	TSubclassOf<AMinion> MinionClass;

	// 生成小兵的出生点列表
	UPROPERTY(EditAnywhere, Category = "Spawn", meta = (DisplayName = "生成小兵的出生点列表"))
	TArray<APlayerStart*> SpawnSpots;

	// 下一个出生点索引
	int32 NextSpawnSpotIndex = -1;

	// 获取下一个出生点（轮流分配）
	const APlayerStart* GetNextSpawnSpot();

	// 生成一组小兵(优先用对象池)
	void SpawnNewGroup();

	// 生成指定数量新小兵
	void SpawnNewMinions(int Amt);

	// 从池中获取可用小兵
	AMinion* GetNextAvailableMinion() const;

	// 生成组的定时器句柄
	FTimerHandle SpawnIntervalTimerHandle;
};
