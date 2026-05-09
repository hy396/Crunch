// 幻雨喜欢小猫咪


#include "AI/MinionBarrack.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/PlayerStart.h"

// Sets default values
AMinionBarrack::AMinionBarrack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();
	// 仅在服务器上定时生成小兵
	if (HasAuthority())
	{
		// 设置定时器，定时批量生成小兵
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &AMinionBarrack::SpawnNewGroup, GroupSpawnInterval, true);
	}
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0) return nullptr;

	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0;
	}
	// 返回出生点
	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnNewGroup()
{
	// 需要生成的小兵数量
	int32 i = MinionPerGroup;

	while (i > 0)
	{
		// 获取出生点变换
		FTransform SpawnTransform = GetActorTransform();
		// 获取下一个出生点
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		// 优先复用对象池中的非激活小兵
		AMinion* NextAvailableMinion = GetNextAvailableMinion();
		// 对象池内没有可以用的小兵了就退出循环，生成一个新的小兵
		if (!NextAvailableMinion) break;

		NextAvailableMinion->SetActorTransform(SpawnTransform);
		NextAvailableMinion->SetGoal(Goal);
		// TODO 2026/05/10 2:18 确认复用小兵的Goal是否有效、是否成功激活
		UE_LOG(LogTemp, Warning, TEXT("[MinionBug] 复用小兵: %s | Goal=%s"),
			*NextAvailableMinion->GetName(), Goal ? *Goal->GetName() : TEXT("NULL"));
		NextAvailableMinion->Activate();
		--i;
	}
	
	// 如果对象池不够，则新建剩余数量的小兵
	SpawnNewMinions(i);
}

void AMinionBarrack::SpawnNewMinions(int Amt)
{
	if (Amt <= 0) return;
	for (int32 i = 0; i < Amt; ++i)
	{
		// 获取出生点变换
		FTransform SpawnTransform = GetActorTransform();
		// 获取下一个出生点
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		// 生成小兵
		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		
		// 设置小兵的队伍ID
		NewMinion->SetGenericTeamId(BarrackTeamId);

		// 完成小兵的生成
		NewMinion->FinishSpawning(SpawnTransform);

		// 设置小兵的目标
		NewMinion->SetGoal(Goal);
		// TODO 2026/05/10 2:18 检查新生小兵是否成功创建且Goal被正确设置
		UE_LOG(LogTemp, Warning, TEXT("[MinionBug] 新生小兵: %s | Goal=%s | TeamID=%d"),
			*NewMinion->GetName(), Goal ? *Goal->GetName() : TEXT("NULL"), BarrackTeamId.GetId());
		// 添加小兵到小兵池中
		MinionPool.Add(NewMinion);
	}
}

void AMinionBarrack::StopSpawning()
{
	// TODO 2026/05/10 2:18 游戏结束时停止小兵生成，防止结束后还在繁殖
	GetWorldTimerManager().ClearTimer(SpawnIntervalTimerHandle);
	UE_LOG(LogTemp, Warning, TEXT("[MinionBug] %s StopSpawning — 生成定时器已清除"), *GetName());

	// 停止场上所有活跃小兵的行为树和移动
	for (AMinion* M : MinionPool)
	{
		if (M && M->IsActive())
		{
			if (AAIController* AIC = Cast<AAIController>(M->GetController()))
			{
				AIC->StopMovement();
				if (UBrainComponent* Brain = AIC->GetBrainComponent())
				{
					Brain->StopLogic("GameOver");
				}
			}
		}
	}
}

AMinion* AMinionBarrack::GetNextAvailableMinion() const
{
	for (AMinion* Minion : MinionPool)
	{
		if (Minion && !Minion->IsActive())
		{
			return Minion;
		}
	}
	return nullptr;
}

