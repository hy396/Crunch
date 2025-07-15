// 幻雨喜欢小猫咪


#include "Animations/CAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Core/TGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"

void UCAnimInstance::NativeInitializeAnimation()
{
	// 获取Owner转换为角色
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		// 获取Owner的移动组件
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		// 监听瞄准标签
		OwnerASC->RegisterGameplayTagEvent(TGameplayTags::Stats_Aim).AddUObject(this, &UCAnimInstance::OwnerAimTagChanged);
	}
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();

		// 获取当前身体旋转角度与上一帧的差值，用于计算旋转变化率
		FRotator BodyRotator = OwnerCharacter->GetActorRotation();
		FRotator BodyRotatorDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotator, BodyPrevRotator);
		BodyPrevRotator = BodyRotator;

		// 计算 yaw 轴旋转速度（每秒旋转角度）
		YawSpeed = BodyRotatorDelta.Yaw / DeltaSeconds;

		// 如果偏航角速度为0，则使用特定的速度进行平滑插值
		float YawLerpSpeed = YawSpeedSmoothLerpSpeed;
		if (YawSpeed == 0)
		{
			YawLerpSpeed = YawSpeedLerpToZeroSpeed;
		}

		// 使用线性插值（FInterpTo）对 yaw 速度进行平滑处理，提高动画过渡质量
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawLerpSpeed);

		// 获取拥有者角色的基础瞄准旋转
		FRotator ControlRotator = OwnerCharacter->GetBaseAimRotation();
		// 计算当前帧的旋转变化量，并将其归一化，以便在多个帧之间平滑旋转
		LookRotatorOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotator, BodyPrevRotator);

		// 获取当前帧的 fwd/right 速度
		FwdSpeed = Velocity.Dot(ControlRotator.Vector());
		RightSpeed = -Velocity.Dot(ControlRotator.Vector().Cross(FVector::UpVector));
	}

	if (OwnerMovementComponent)
	{
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
}

void UCAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	
}

bool UCAnimInstance::ShouldDoFullBody() const
{
	// 速度为0 并且 不处于瞄准状态
	return (GetSpeed() <= 0) && !(GetIsAiming());
}

void UCAnimInstance::OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount != 0;
}
