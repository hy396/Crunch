// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Crunch/Private/Character/CCharacter.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "GAS/Core/CHeroAttributeSet.h"
#include "CPlayerCharacter.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()
public:
	ACPlayerCharacter();

	/** 
     * 在客户端重新开始时调用
     * 用于处理玩家角色的重生或初始化逻辑
     */
    virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	TObjectPtr<class UCameraComponent> ViewCamera;

	/**
	 * 获取角色视线右侧的方向向量
	 * 
	 * @return FVector ViewCamera的右侧方向向量
	 */
	FVector GetLookRightDir() const;
	
	/**
	 * 获取角色视线向前的方向向量
	 * 
	 * @return FVector ViewCamera的前向方向向量
	 */
	FVector GetLookFwdDir() const;
	
	/**
	 * 计算角色向前移动的方向向量
	 * 通过右侧方向向量与向上方向向量的叉积获取前向移动方向
	 * 
	 * @return FVector 前向移动方向向量
	 */
	FVector GetMoveFwdDir() const;
#pragma region Gameplay Ability
private:
	// 瞄准状态变化时回调
	virtual void OnAimStateChanged(bool bIsAiming) override;
	UPROPERTY()
	TObjectPtr<UCHeroAttributeSet> HeroAttributeSet;
#pragma endregion
#pragma region Input
private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveInputAction;

	// 技能升级触发键
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LearnAbilityLeaderAction;

	// 技能输入
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<ECAbilityInputID, TObjectPtr<UInputAction>> GameplayAbilityInputActions;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> GameplayInputMappingContext;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);
	// 技能升级触发按下
	void LearnAbilityLeaderDown(const FInputActionValue& InputActionValue);
	// 技能升级触发抬起
	void LearnAbilityLeaderUp(const FInputActionValue& InputActionValue);
	// 是否按下技能升级键
	bool bIsLearnAbilityLeaderDown = false;
	
	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID);
	// 玩家的输入开关
	void SetInputEnabledFromPlayerController(bool bEnabled);
#pragma endregion
#pragma region 眩晕(Stun)
private:
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;
#pragma endregion

#pragma region 死亡和复活 (Death and Respawn)
	virtual void OnDead() override;
	virtual void OnRespawn() override;
#pragma endregion

#pragma region 摄像机视角
private:
	// 瞄准时相机的本地偏移量
	UPROPERTY(EditDefaultsOnly, Category = view)
	FVector CameraAimLocalOffset;

	// 相机插值速度
	UPROPERTY(EditDefaultsOnly, Category = view)
	float CameraLerpSpeed = 20.f;
    
	// 相机插值定时器句柄
	FTimerHandle CameraLerpTimerHandle;

	// 插值相机到目标本地偏移位置
	void LerpCameraToLocalOffsetLocation(const FVector& Goal);

	// 相机插值Tick回调
	void TickCameraLocalOffsetLerp(FVector Goal);
#pragma endregion
};
