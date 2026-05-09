// 幻雨喜欢小猫咪


#include "Minion.h"

#include "AIController.h"
#include "BrainComponent.h"
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
    //     AICon->StopMovement();
    //     AICon->RestartLogic();
    // }
	// TODO 2026/05/10 2:18 确认小兵复活后AI控制器存在、行为树状态正常
	if (AAIController* AICon = GetController<AAIController>())
	{
		UE_LOG(LogTemp, Warning, TEXT("[MinionBug] %s Activate | AIController=%s | BrainRunning=%d"),
			*GetName(),
			AICon ? *AICon->GetName() : TEXT("NULL"),
			(AICon && AICon->GetBrainComponent()) ? AICon->GetBrainComponent()->IsRunning() : -1);
	}
}

void AMinion::SetGoal(AActor* Goal)
{
	if (AAIController* AIController = GetController<AAIController>())
	{
		if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			// 修改黑板组件中对应键目标的值
			BlackboardComponent->SetValueAsObject(GoalBlackboardKeyName, Goal);
			// TODO 2026/05/10 2:18 检查Goal是否成功写入黑板
			UE_LOG(LogTemp, Warning, TEXT("[MinionBug] %s SetGoal | Goal=%s | BB Key=%s | AIC=%s"),
				*GetName(),
				Goal ? *Goal->GetName() : TEXT("NULL"),
				*GoalBlackboardKeyName.ToString(),
				*AIController->GetName());
		}
		else
		{
			// TODO 2026/05/10 2:18 黑板组件为空——可能是控制器尚未完成初始化
			UE_LOG(LogTemp, Error, TEXT("[MinionBug] %s SetGoal 失败——黑板组件为空! AIC=%s"),
				*GetName(), *AIController->GetName());
		}
	}
	else
	{
		// TODO 2026/05/10 2:18 AI控制器为空——可能发生在非激活状态下调用SetGoal
		UE_LOG(LogTemp, Warning, TEXT("[MinionBug] %s SetGoal 失败——AI控制器为空! Goal=%s"),
			*GetName(), Goal ? *Goal->GetName() : TEXT("NULL"));
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
