// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	/**
	 * 动画实例原生初始化覆盖点
	 * 用于执行动画实例的初始化逻辑，在实例创建时调用
	 */
	virtual void NativeInitializeAnimation() override;
	
	/**
	 * 动画实例原生更新覆盖点
	 * 主要用于在游戏线程中收集动画数据，实际计算建议放在 NativeThreadSafeUpdateAnimation 中
	 * 
	 * @param DeltaSeconds 当前帧与上一帧之间的时间间隔（秒）
	 */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	/**
	 * 动画实例线程安全更新覆盖点
	 * 在工作线程中执行，适用于链接动画实例的预处理计算
	 * 仅当宿主节点相关时才会调用
	 * 
	 * @param DeltaSeconds 当前帧与上一帧之间的时间间隔（秒）
	 */
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	// 获取角色速度
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	// 是否正在移动
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0; }

	// 是否静止
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsNotMoving() const { return Speed == 0; }

	// 获取当前Yaw旋转速度
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	// 获取前进方向速度
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetFwdSpeed() const { return FwdSpeed; }

	// 获取横向速度
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightSpeed() const { return RightSpeed; }
	
	// 获取平滑处理后的Yaw速度
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

	// 是否处于跳跃状态
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }

	// 是否在地面上
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsOnGround() const { return !bIsJumping; }

	// 是否处于瞄准状态
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	
	// 获取视角Yaw偏移
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookYawOffset() const { return LookRotatorOffset.Yaw; }

	// 获取视角Pitch偏移
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookPitchOffset() const { return LookRotatorOffset.Pitch; }

	// 是否需要执行全身动画
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	bool ShouldDoFullBody() const;
private:
	// 监听拥有者瞄准标签变化
	void OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount);
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerMovementComponent;

	float Speed;
	/**
	 * 当前帧的偏航角速度
	 * 用于计算角色在水平面上的旋转速度
	 */
	float YawSpeed;
	/**
	 * 平滑后的偏航角速度
	 *  用于在多个帧之间平滑偏航角速度，以减少抖动
	 */
	float SmoothedYawSpeed;

	// 前进方向速度
	float FwdSpeed;
	// 横向速度
	float RightSpeed;
	
	// 是否跳跃
	bool bIsJumping;
	// 是否瞄准
	bool bIsAiming;
	
	/**
	 * 平滑插值的速度
	 * 控制偏航角速度从当前值平滑过渡到零的速度
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedSmoothLerpSpeed = 1.f;

	/**
	 * 当偏航角速度降到零时使用的插值速度
	 * 用于控制偏航角速度从当前值平滑过渡到零的加速度
	 */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedLerpToZeroSpeed = 30.f;

	/**
	 * 前一帧的角色旋转
	 * 用于计算当前帧的旋转变化量
	 */
	FRotator BodyPrevRotator;

	// 视角旋转偏移
	FRotator LookRotatorOffset;

};
