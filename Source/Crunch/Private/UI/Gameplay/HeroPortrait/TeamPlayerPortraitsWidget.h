// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
// #include "Components/VerticalBox.h"
#include "TeamPlayerPortraitsWidget.generated.h"

class UPlayerPortraitWidget;
class AMPlayerState;
class ACCharacter;
class UPDA_CharacterDefinition;

/**
 * 团队玩家头像控件
 * 用于显示团队中所有玩家的头像和状态信息
 */
UCLASS()
class CRUNCH_API UTeamPlayerPortraitsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 控件初始化
	virtual void NativeConstruct() override;

	// 初始化团队头像显示
	void InitializeTeamPortraits();
private:
	//HorizontalBox
	// 己方单位头像控件容器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> PlayerPortraitContainer;
	// 敌方单位头像控件容器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> EnemyPortraitContainer;

	// 本地玩家状态
	UPROPERTY()
	TObjectPtr<AMPlayerState> LocalPlayerState;

	// 玩家状态数组，用于存储当前跟踪的玩家状态
	TArray<TWeakObjectPtr<AMPlayerState>> TrackedPlayerStates;

	// 玩家头像控件
	UPROPERTY(EditDefaultsOnly, Category = "TeamPortraits")
	TSubclassOf<UPlayerPortraitWidget> PlayerPortraitWidgetClass;

	// 定时器回调函数
	UFUNCTION()
	void OnTimerCallback();
    
	// 定时器句柄
	FTimerHandle RefreshTimerHandle;
};