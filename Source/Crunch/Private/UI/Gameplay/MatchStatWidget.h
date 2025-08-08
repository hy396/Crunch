// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

class AStormCore;
class UTextBlock;
class UImage;
/**
 * 比赛统计信息展示控件
 * 用于显示比赛进度、队伍影响力等数据
 */
UCLASS()
class CRUNCH_API UMatchStatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构建时回调（初始化控件和绑定事件）
	virtual void NativeConstruct() override;
private:
	// 进度条刷新间隔
	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	float ProgressUpdateInterval = 0.5f;

	// 动态材质参数名（用于进度条）
	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	FName ProgressDynamicMaterialParamName = "Progress";

	// 进度条图片控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ProgressImage;

	// 队伍一影响力文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TeamOneCountText;

	// 队伍二影响力文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TeamTwoCountText;

	// 当前比赛核心对象
	UPROPERTY()
	TObjectPtr<AStormCore> StormCore;

	// 更新队伍影响力显示
	void UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount);

	// 比赛结束处理
	void MatchFinished(AActor* ViewTarget, int WinningTeam);

	// 更新进度条显示
	void UpdateProgress();

	// 进度条刷新定时器句柄
	FTimerHandle UpdateProgressTimerHandle;
};
