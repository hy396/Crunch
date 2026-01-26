// 幻雨喜欢小猫咪


#include "CAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/Core/TGameplayTags.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"

// 该Super构造函数中的SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent")是用于设置默认的寻路组件
// 因为默认的寻路组件是PathFollowingComponent，而我们需要的是CrowdFollowingComponent，所以需要在构造函数中设置默认的寻路组件
ACAIController::ACAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 配置视觉感知的关联检测设置：
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;		// 只检测敌人
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;	// 不检测中立
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;	// 不检测友方

	// 设置视觉感知参数：
	SightConfig->SightRadius = 1000.f;					// 视野范围
	SightConfig->LoseSightRadius = 1200.f;				// 失去视野范围
	SightConfig->PeripheralVisionAngleDegrees = 180.f;	// 视野角度

	SightConfig->SetMaxAge(5.f);						// 记忆时间
	
	// 配置AI感知组件的视觉感知
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());	// 设置主要感官为视觉
	// 感知目标更新
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);
	// 感知目标遗忘
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ACAIController::TargetForgotten);
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
		// 注意这里：我们在生成AI角色时，要先赋予（确定）角色的队伍ID，然后再调用FinishSpawning去生成角色
		// 这样OnPossess这里才能正确获取到角色的队伍ID

		// 同时，AI在生成时，可能已经有某些感知信息了，所以我们需要让感知组件更新一次
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}
	
	// 订阅GAS的Tag生成或者移除事件，以便在角色死亡时禁用AI感知或者重生时启用AI感知
	if (UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		PawnASC->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ACAIController::PawnDeadTagUpdated);
		PawnASC->RegisterGameplayTagEvent(TGameplayTags::Stats_Stun).AddUObject(this, &ACAIController::PawnStunTagUpdated);
	}
}

ETeamAttitude::Type ACAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other))
	{
		const FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();

		if (FGenericTeamId MyTeamID = GetGenericTeamId(); MyTeamID == OtherTeamID) 
			return ETeamAttitude::Friendly;
		else if (MyTeamID == FGenericTeamId::NoTeam || OtherTeamID == FGenericTeamId::NoTeam)
			return ETeamAttitude::Neutral;
		else
			return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Neutral;
}

void ACAIController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	Super::SetGenericTeamId(InTeamID);

	ClearAndDisableAllSenses();
	EnableAllSenses();
}

void ACAIController::SetSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees)
{
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
	AIPerceptionComponent->RequestStimuliListenerUpdate();		// 请求更新感知监听器
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	// 运行行为树
	RunBehaviorTree(BehaviorTree);

	if (UCrowdFollowingComponent* CrowdFollowingComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		// 是否启用人群避让
		CrowdFollowingComp->SetCrowdSimulationState(bEnableCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::ObstacleOnly);

		// 设置人群避让质量
		switch (DetourCrowdAvoidanceQuality)
		{
		case 1:
			CrowdFollowingComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
			break;
		case 2:
			CrowdFollowingComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
			break;
		case 3:
			CrowdFollowingComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
			break;
		case 4:
			CrowdFollowingComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
			break;
		default: break;
		}

		// 设置避让组：1表示我们当前的AI应该对其他AI（EnemyAI，因为我们此前设置的团队ID 1 就是我们的EnemyTeam)进行避让
		// CrowdFollowingComp->SetAvoidanceGroup(1);				// 设置避让组
		// CrowdFollowingComp->SetGroupsToAvoid(1);		// 设置需要避让的组

		// 设置人群避让半径
		CrowdFollowingComp->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
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
		if (Actors.Num() > 0)
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
		bIsPawnDead = true;
	}
	else
	{
		GetBrainComponent()->StartLogic(); // 重新启动AI逻辑
		EnableAllSenses(); // 启用感知系统
		bIsPawnDead = false;
	}
}

void ACAIController::PawnStunTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (bIsPawnDead) return;

	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Stun");
	}else
	{
		GetBrainComponent()->StartLogic();
	}
}
