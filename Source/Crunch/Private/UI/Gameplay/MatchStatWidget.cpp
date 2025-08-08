// 幻雨喜欢小猫咪


#include "MatchStatWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/StormCore.h"
#include "Kismet/GameplayStatics.h"

void UMatchStatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 获取场景中的风暴核心
	StormCore = Cast<AStormCore>(UGameplayStatics::GetActorOfClass(this, AStormCore::StaticClass()));

	if (StormCore)
	{
		// 绑定影响力更新事件
		StormCore->OnTeamInfluenceCountUpdated.AddUObject(this, &UMatchStatWidget::UpdateTeamInfluence);
		// 绑定比赛结束事件
		StormCore->OnGoalReachedDelegate.AddUObject(this, &UMatchStatWidget::MatchFinished);
		// 启动定时器，定时刷新进度条
		GetWorld()->GetTimerManager().SetTimer(UpdateProgressTimerHandle, this, &UMatchStatWidget::UpdateProgress, ProgressUpdateInterval, true);
	}
}

void UMatchStatWidget::UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount)
{
	// 刷新团队影响力文本
	TeamOneCountText->SetText(FText::AsNumber(TeamOneCount));
	TeamTwoCountText->SetText(FText::AsNumber(TeamTwoCount));
}

void UMatchStatWidget::MatchFinished(AActor* ViewTarget, int WinningTeam)
{
	// 根据获胜队伍设置进度条
	float Progress = WinningTeam == 0 ? 1 : 0;

	// 停止进度条刷新定时器
	GetWorld()->GetTimerManager().ClearTimer(UpdateProgressTimerHandle);
	// 设置进度条材质参数
	ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(ProgressDynamicMaterialParamName, Progress);
}

void UMatchStatWidget::UpdateProgress()
{
	if (StormCore)
	{
		// 获取当前进度并设置到进度条材质
		float Progress = StormCore->GetProgress();
		ProgressImage->GetDynamicMaterial()->SetScalarParameterValue(ProgressDynamicMaterialParamName, Progress);
	}
}
