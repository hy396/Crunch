// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"	// 团队管理接口
#include "UI/Gameplay/Chat/Interface/ChatInterface.h"	// 聊天接口
#include "NumberPopComponent_NiagaraText.h"
#include "CPlayerController.generated.h"

class UChatWidget;
class UGameplayWidget;
class AActor;

/**
 * 
 */
UCLASS()
class ACPlayerController : public APlayerController, public IGenericTeamAgentInterface, public IChatInterface
{
	GENERATED_BODY()
public:
	ACPlayerController();
	// 玩家控制器控制一个Pawn时调用（该函数仅在服务器中调用）
	virtual void OnPossess(APawn* NewPawn) override;
	// 玩家控制器控制一个Pawn时调用（仅在客户端中调用，也在监听服务器（listening server）上调用）
	virtual void AcknowledgePossession(APawn* P) override;
	// 设置输入组件
	virtual void SetupInputComponent() override;

	// 设置团队ID
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	// 获取团队ID
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 比赛结束处理
	void MatchFinished(AActor* ViewTarget, int WiningTeam);
	// 销毁
	// virtual void BeginDestroy() override;
	// 在每个客户端显示伤害数值
//	UFUNCTION(Client, Reliable)
//	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit);

	// 在每个客户端显示伤害数值
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit, FGameplayTag DamageType);

	// 聊天系统相关函数
	// 服务器RPC：发送聊天消息
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);

	// 客户端RPC：接收聊天消息
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatMessage(const FChatMessage& Message);

	// IChatInterface 接口实现
	virtual void SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType) override;
	virtual void ReceiveChatMessageFromServer(const FChatMessage& Message) override;
private:
	// 客户端比赛结束处理
	UFUNCTION(Client, Reliable)
	void Client_MatchFinished(AActor* ViewTarget, int WiningTeam);

	// 生成游戏主界面控件
	void SpawnGameplayWidget();

	// 比赛结束视角切换时长
	UPROPERTY(EditDefaultsOnly, Category="View")
	float MatchFinishViewBlendTimeDuration = 2.f;
	
	UPROPERTY()
	TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

	UPROPERTY(EditAnywhere, Category = "Components")
	TSubclassOf<UNumberPopComponent_NiagaraText> NumberPopComponentClass;
	// 对象池管理
	// UPROPERTY()
	// TArray<TObjectPtr<UNumberPopComponent_NiagaraText>> ActiveNumberPops;
	// UFUNCTION()
	// void HandleTargetActorDestroyed(AActor* DestroyedActor);

	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;
	
	// 团队ID
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;

	// UI输入映射
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> UIInputMapping;

	// 商店打开/关闭的按键
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ShopToggleInputAction;

	// 游戏菜单的开关/关闭
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleGameplayMenuAction;

	// 聊天系统输入动作
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleChatInputAction;
	
	// 商店的开关/关闭
	UFUNCTION()
	void ToggleShop();

	// 游戏菜单的开关/关闭
	UFUNCTION()
	void ToggleGameplayMenu();

	// 聊天窗口切换
	UFUNCTION()
	void ToggleChat();
	
	// 显示游戏结果
	void ShowWinLoseState();


	// 注册单位
	void RegisterUnitForMinimap(AActor* Unit);

};
