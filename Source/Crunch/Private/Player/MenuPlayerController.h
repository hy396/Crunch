// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 菜单界面专用玩家控制器
 * 负责菜单UI的生成与管理
 */
UCLASS()
class CRUNCH_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
public:	
	// 游戏开始时调用
	virtual void BeginPlay() override;

	// 玩家状态同步时调用
	virtual void OnRep_PlayerState() override;

private:
	// 菜单界面类
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UUserWidget> MenuWidgetClass;

	// 菜单界面实例
	UPROPERTY()
	TObjectPtr<UUserWidget> MenuWidget;

	// 生成菜单界面
	void SpawnWidget();
};
