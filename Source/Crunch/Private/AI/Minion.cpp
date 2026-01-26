// 幻雨喜欢小猫咪


#include "Minion.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Gameplay/OverHeadStatsGauge.h"


void AMinion::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	Super::SetGenericTeamId(NewTeamId);
	PickSkinBasedOnTeamID();
}

bool AMinion::IsActive() const
{
	return !IsDead();
}

void AMinion::Activate()
{
	// 移除死亡标签，复活
	RespawnImmediately();

	// if (AAIController* AICon = GetController<AAIController>())
	// {
	// 	AICon->StopMovement();
	// 	AICon->RestartLogic();
	// }
}

void AMinion::SetGoal(AActor* Goal)
{
	if (AAIController* AIController = GetController<AAIController>())
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			// 修改黑板组件中对应键目标的值
			BlackboardComponent->SetValueAsObject(GoalBlackboardKeyName, Goal);
		}
	}
}

void AMinion::PickSkinBasedOnTeamID()
{
	TObjectPtr<USkeletalMesh>* Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(*Skin);
		// UE_LOG(LogTemp, Warning, TEXT("当前角色 TeamID: %u"), GetGenericTeamId().GetId());
		// 设置头顶UI颜色
		// SetOverHeadWidgetColor();
	}
	
}

void AMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}
