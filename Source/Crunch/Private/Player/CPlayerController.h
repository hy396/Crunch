// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"
class UGameplayWidget;
/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 玩家控制器控制一个Pawn时调用（该函数仅在服务器中调用）
	virtual void OnPossess(APawn* NewPawn) override;
	// 玩家控制器控制一个Pawn时调用（仅在客户端中调用，也在监听服务器（listening server）上调用）
	virtual void AcknowledgePossession(APawn* P) override;

private:
	void SpawnGameplayWidget();
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;
};
