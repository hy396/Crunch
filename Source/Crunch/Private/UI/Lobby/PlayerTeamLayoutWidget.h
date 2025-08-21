// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "PlayerTeamLayoutWidget.generated.h"

class UHorizontalBox;
class UPlayerTeamSlotWidget;
/**
 * 队伍布局UI控件 - 管理并显示两支队伍（红队/蓝队）的玩家槽位
 * 功能：
 *   - 动态创建队伍槽位控件
 *   - 将槽位分配到两个水平布局框（队伍1和队伍2）
 *   - 根据玩家选择数据更新所有槽位
 */
UCLASS()
class CRUNCH_API UPlayerTeamLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	/**
	 * 更新所有玩家选择状态
	 * @param PlayerSelections 玩家选择数据数组（包含所有玩家的选择信息）
	 */
	void UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections);

private:
	// 槽位控件之间的间距
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float PlayerTeamWidgetSlotMargin = 5.f;

	// 单个玩家槽位控件的类引用（蓝图类）
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UPlayerTeamSlotWidget> PlayerTeamSlotWidgetClass;

	// 队伍1的槽位水平布局框
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TeamOneLayoutBox;

	// 队伍2的槽位水平布局框
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TeamTwoLayoutBox;

	// 槽位数组
	UPROPERTY()
	TArray<TObjectPtr<UPlayerTeamSlotWidget>> TeamSlotWidgets;
};
