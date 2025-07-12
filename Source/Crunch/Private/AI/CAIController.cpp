// 幻雨喜欢小猫咪


#include "CAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/Core/TGameplayTags.h"
#include "Perception/AIPerceptionComponent.h"

ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 配置视觉感知的关联检测设置：
	// - 检测敌人：启用
	// - 检测中立目标：禁用
	// - 检测友方单位：禁用
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	// 设置视觉感知参数：
	// - 视野半径
	SightConfig->SightRadius = 1000.f;
	// - 失去视野半径（当目标超出此范围时AI会认为失去目标）
	SightConfig->LoseSightRadius = 1200.f;
	// - 感知信息的最大保存时间：5秒
	SightConfig->SetMaxAge(5.f);
	// - 周边视觉角度：180度
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	
	// 配置AI感知组件的视觉感知
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	// 感知目标更新
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	// 感知目标遗忘
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten);
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(InPawn);
	if (PawnTeamInterface)
	{
		SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}

	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn);
	if (PawnASC)
	{
		PawnASC->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACAIController::PawnDeadTagUpdated);
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	// 运行行为树
	RunBehaviorTree(BehaviorTree);
}

void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	// 检查是否成功感知到目标
	if (Stimulus.WasSuccessfullySensed())
	{
		// 如果当前没有目标，则设置当前目标为感知到的目标
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}else
	{
		// 忘记已死亡的目标
		ForgetActorIfDead(TargetActor);
	}
}

void ACAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor) return;

	// 如果丢失的是当前的目标，则切换到下一个感知到的目标
	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

const UObject* ACAIController::GetCurrentTarget() const
{
	// 获取黑板组件
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		// 从黑板组件中根据键名获取目标对象
		return BlackboardComponent->GetValueAsObject(TargetBlackboardKeyName);
	}
	// 如果黑板组件不存在，返回nullptr
	return nullptr;
}

void ACAIController::SetCurrentTarget(AActor* NewTarget)
{
	// 获取黑板组件
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	// 如果黑板组件为空，则不执行任何操作
	if (!BlackboardComponent) return;

	// 如果新目标不为空，则在黑板上设置新目标
	if (NewTarget)
	{
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
	}
	// 如果新目标为空，则从黑板上清除当前目标
	else
	{
		BlackboardComponent->ClearValue(TargetBlackboardKeyName);
	}
}

AActor* ACAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		// 存储感知到的Actor
		TArray<AActor*> Actors;
		// 获取感知到的敌对Actor并存储到数组中
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		// 如果敌对Actor的数量不为零
		if (Actors.Num() != 0)
		{
			// 返回第一个
			return Actors[0];
		}
	}

	return nullptr;
}

void ACAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC) return;

	// 检测Actor是否具有死亡标签
	if (ActorASC->HasMatchingGameplayTag(TGameplayTags::Stats_Dead))
	{
		// 遍历AI感知组件的感知数据容器
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComponent->GetPerceptualDataIterator(); Iter; ++Iter)
		{
			// 查找与目标Actor匹配的感知数据条目
			if (Iter->Key == ActorToForget)
			{
				// 将感知刺激年龄设置为最大值，触发AI遗忘逻辑
				// 这会使得该Actor从AI的感知列表中被移除
				for (FAIStimulus& Stimuli : Iter->Value.LastSensedStimuli)
				{
					Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
				}
				break; // 找到匹配项后跳出循环，避免不必要的遍历
			}
		}
	}
}

void ACAIController::ClearAndDisableAllSenses()
{
	// 老化所有感知刺激到最大时间值，强制标记为过期状态
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

	// 遍历所有感知配置并禁用它们
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false);
	}
	if (GetBlackboardComponent())
	{
		// 消除黑板中的目标值
		GetBlackboardComponent()->ClearValue(TargetBlackboardKeyName);
	}
}

void ACAIController::EnableAllSenses()
{
	// 遍历所有感知配置并启用它们
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true);
	}
}

void ACAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Dead"); // 停止死亡状态下的逻辑
		ClearAndDisableAllSenses(); // 清除感知数据
	}
	else
	{
		GetBrainComponent()->StartLogic(); // 重新启动AI逻辑
		EnableAllSenses(); // 启用感知系统
	}
}
