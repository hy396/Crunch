// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"	// 团队管理接口
#include "NumberPopComponent_NiagaraText.h"
#include "CPlayerController.generated.h"
class UGameplayWidget;
/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ACPlayerController();
	// 玩家控制器控制一个Pawn时调用（该函数仅在服务器中调用）
	virtual void OnPossess(APawn* NewPawn) override;
	// 玩家控制器控制一个Pawn时调用（仅在客户端中调用，也在监听服务器（listening server）上调用）
	virtual void AcknowledgePossession(APawn* P) override;

	// 设置团队ID
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	// 获取团队ID
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 在每个客户端显示伤害数值
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit);

private:
	void SpawnGameplayWidget();
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UNumberPopComponent_NiagaraText> NumberPopComponent;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;
	
	// 团队ID
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
