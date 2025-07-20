// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Components/WidgetComponent.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GAS/Core/CAbilitySystemComponent.h"
#include "GAS/Core/CAttributeSet.h"
#include "Interaction/CombatInterface.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Player/NumberPopComponent_NiagaraText.h"
#include "CCharacter.generated.h"

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface	//, public ICombatInterface
{
	GENERATED_BODY()

public:
	ACCharacter();
	// 服务器中初始化
	void ServerSideInit();
	// 客户端中初始化
	void ClientSideInit();
	// 判断该角色是否由本地玩家控制
	bool IsLocallyControlledByPlayer() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 获取技能表
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
	// 头顶UI颜色
	void SetOverHeadWidgetColor();
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	// TObjectPtr<UNumberPopComponent_NiagaraText> NumberPopComponent;

	// 添加Niagara文字
	// virtual void AddNiagaraText_Implementation(const FNumberPopRequest& NewRequest) override;
protected:
	virtual void BeginPlay() override;
	// 只在服务器执行
	virtual void PossessedBy(AController* NewController) override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
#pragma region GAS组件相关
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 在服务器端向自身发送游戏事件
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
private:
	// 绑定GAS属性改变委托
	void BindGASChangeDelegates();
	// 死亡标签的更新
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);
	// 眩晕标签的更新
	void StunTagUpdated(const FGameplayTag Tag, int32 NewCount);
	// 瞄准标签变化回调
	void AimTagUpdated(const FGameplayTag Tag, int32 NewCount);
	
	// 设置是否处于瞄准状态
	void SetIsAiming(bool bIsAiming);
	// 瞄准状态变化时回调
	virtual void OnAimStateChanged(bool bIsAiming);

	// 移动速度改变回调
	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);

	// 伤害传入
	// void AttackDamageUpdated(const FOnAttributeChangeData& Data);
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	TObjectPtr<UCAbilitySystemComponent> CAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UCAttributeSet> CAttributeSet;
#pragma endregion

#pragma region UI
private:
	// 头顶UI
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> OverHeadWidgetComponent;
	void ConfigureOverHeadStatusWidget();

	/**
	 * @brief 头顶状态条更新间隔时间（秒）\n
	 * 控制头顶UI显示/隐藏检测的刷新频率，单位为秒。\n
	 * 可在编辑器中调整，默认值为1秒。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityCheckUpdateGap = 1.f;
	/**
	 * @brief 头顶状态条可见性检测的距离平方阈值\n
	 * 当玩家与角色之间的距离平方小于该值时，显示头顶UI；\n
	 * 可在编辑器中调整，默认值为10,000,000。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisibilityRangeSquared = 10000000.f;
	
	// 用于控制头顶状态条可见性更新的定时器句柄
	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;

	/**
	 * @brief 更新头顶状态条可见性\n
	 * 根据玩家与角色之间的距离判断是否启用头顶UI组件的显示。\n
	 * 该方法通过定时器周期性调用。
	 */
	void UpdateHeadGaugeVisibility();

	/**
	 * @brief 设置头顶状态条的启用状态\n
	 * 启用或禁用头顶UI组件的显示。\n
	 * @param bIsEnabled 是否启用头顶UI
	 */
	void SetStatusGaugeEnabled(bool bIsEnabled);
#pragma endregion
#pragma region 眩晕(Stun)
private:
	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	UAnimMontage* StunMontage;

	virtual void OnStun();
	virtual void OnRecoverFromStun();
#pragma endregion
#pragma region 死亡和复活 (Death and Respawn)
public:
	/**
	 * @brief 检查角色当前是否处于死亡状态
	 * 
	 * @return bool true表示角色已死亡，false表示存活
	 */
	bool IsDead() const;

	/**
	 * @brief 立即执行角色重生操作
	 */
	void RespawnImmediately();
private:
	// 相对于网格的变换
	FTransform MeshRelativeTransform;
	
	// 死亡蒙太奇完成时间偏移
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimeShift = -0.8f;
	
	// 死亡蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	TObjectPtr<UAnimMontage> DeathMontage;

	// 死亡蒙太奇计时器句柄
	FTimerHandle DeathMontageTimerHandle;
	// 死亡蒙太奇完成处理
	void DeathMontageFinished();
	// 启用/禁用 布娃娃系统
	void SetRagdollEnabled(bool bIsEnabled);
	
	// 播放死亡动画
	void PlayDeathAnimation();
	
	// 死亡
	void StartDeathSequence();
	// 复活
	void Respawn();

	virtual void OnDead();
	virtual void OnRespawn();
#pragma endregion
#pragma region 团队(Team)
public:
	// 设置团队ID
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	// 获取团队ID
	virtual FGenericTeamId GetGenericTeamId() const override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();
#pragma endregion
#pragma region AI
private:
	// 启用/禁用 AI感知刺激源组件
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled);
	// AI感知刺激源组件
	UPROPERTY()
	TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimuliSourceComponent;
#pragma endregion
};
