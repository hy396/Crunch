// 幻雨喜欢小猫咪


#include "Crunch/Private/Player/CPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ACPlayerCharacter::ACPlayerCharacter()
{
	// 创建并设置摄像机弹簧臂组件
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent()); // 将弹簧臂组件附加到角色的根组件
	CameraBoom->bUsePawnControlRotation = true; // 使用Pawn控制旋转
	//CameraBoom->ProbeChannel = ECC_SpringArm;
	CameraBoom->TargetArmLength = 800.0f; // 设置弹簧臂的长度为800
	CameraBoom->SetRelativeRotation(FRotator(0.0f, 50.0f, 0.0f));// 设置弹簧臂的高度为50

	// 创建并设置视角摄像机组件
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 将视角摄像机附加到弹簧臂组件的指定插槽

	// 设置角色旋转行为
	bUseControllerRotationYaw = false; // 禁用控制器的Yaw旋转
	GetCharacterMovement()->bOrientRotationToMovement = true; // 旋转角色以匹配移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // 设置角色的旋转速率
}

void ACPlayerCharacter::PawnClientRestart()
{
	// 调用基类的PawnClientRestart函数以执行默认的重启逻辑
	Super::PawnClientRestart();

	// 检查当前角色是否拥有玩家控制器
	if (const APlayerController* OwningPlayerController = GetController<APlayerController>())
	{
		// 获取与玩家控制器关联的本地玩家对象
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// 移除现有的输入映射上下文
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			// 重新添加输入映射上下文，优先级为0（确保高优先级）
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 绑定跳、看、走
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleMoveInput);

		// 绑定技能输入
		for (const TPair<ECAbilityInputID, TObjectPtr<UInputAction>>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleAbilityInput, InputActionPair.Key);
		}
	}
}

FVector ACPlayerCharacter::GetLookRightDir() const
{
	return ViewCamera->GetRightVector();
}

FVector ACPlayerCharacter::GetLookFwdDir() const
{
	return ViewCamera->GetForwardVector();
}

FVector ACPlayerCharacter::GetMoveFwdDir() const
{
	return FVector::CrossProduct(GetLookRightDir(), FVector::UpVector);
}

void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	// 将输入值从FVector2D转换为FVector2D类型
	const FVector2D InputVal = InputActionValue.Get<FVector2D>();

	// 根据输入值调整玩家的俯仰角(pitch)
	AddControllerPitchInput(-InputVal.Y);
	// 根据输入值调整玩家的偏航角(yaw)
	AddControllerYawInput(InputVal.X);
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	// 如果玩家处于聚焦模式，则不处理移动输入
	// if (GetIsInFocusMode())
	// 	return;
	//
	// 获取输入值并转换为FVector2D类型
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	// 对输入值进行归一化处理
	InputVal.Normalize();
	// 根据处理后的输入值更新玩家的移动方向
	AddMovementInput(GetMoveFwdDir() * InputVal.Y + GetLookRightDir() * InputVal.X);
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();

	// 按下
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed(static_cast<int32>(InputID));
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased(static_cast<int32>(InputID));
	}
}


