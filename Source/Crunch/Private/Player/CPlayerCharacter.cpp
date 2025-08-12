// 幻雨喜欢小猫咪


#include "Crunch/Private/Player/CPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Crunch/Crunch.h"
#include "GAS/Core/TGameplayTags.h"


ACPlayerCharacter::ACPlayerCharacter()
{
	// 创建并设置摄像机弹簧臂组件
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent()); // 将弹簧臂组件附加到角色的根组件
	CameraBoom->bUsePawnControlRotation = true; // 使用Pawn控制旋转
	CameraBoom->ProbeChannel = ECC_SPRING_ARM;
	CameraBoom->TargetArmLength = 800.0f; // 设置弹簧臂的长度为800
	CameraBoom->SetRelativeRotation(FRotator(0.0f, 50.0f, 0.0f));// 设置弹簧臂的高度为50

	// 创建并设置视角摄像机组件
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 将视角摄像机附加到弹簧臂组件的指定插槽

	// 设置角色旋转行为
	bUseControllerRotationYaw = false; // 禁用控制器的Yaw旋转
	GetCharacterMovement()->bOrientRotationToMovement = true; // 旋转角色以匹配移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // 设置角色的旋转速率

	HeroAttributeSet = CreateDefaultSubobject<UCHeroAttributeSet>(TEXT("HeroAttributeSet"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory Component");
}

void ACPlayerCharacter::PawnClientRestart()
{
	// 调用基类的PawnClientRestart函数以执行默认的重启逻辑
	Super::PawnClientRestart();

	// 检查当前角色是否拥有玩家控制器
	if (APlayerController* OwningPlayerController = GetController<APlayerController>())
	{
		// TODO:暂时处理一下，要删的
		// IGenericTeamAgentInterface* NewPlayerTeamInterfaceCharacter = Cast<IGenericTeamAgentInterface>(OwningPlayerController);
		// SetGenericTeamId(NewPlayerTeamInterfaceCharacter->GetGenericTeamId());
		// //UE_LOG(LogTemp, Warning, TEXT("角色：%s,Id:%u"), *GetName(), GetGenericTeamId().GetId());
		// UE_LOG(LogTemp, Warning, TEXT("角色：%s,Id:%u"), *GetName(), GetGenericTeamId().GetId());

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

		// 按下
		EnhancedInputComponent->BindAction(LearnAbilityLeaderAction, ETriggerEvent::Started, this, &ACPlayerCharacter::LearnAbilityLeaderDown);
		// 抬起
		EnhancedInputComponent->BindAction(LearnAbilityLeaderAction, ETriggerEvent::Completed, this, &ACPlayerCharacter::LearnAbilityLeaderUp);

		// 绑定技能输入
		for (const TPair<ECAbilityInputID, TObjectPtr<UInputAction>>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleAbilityInput, InputActionPair.Key);
		}

		EnhancedInputComponent->BindAction(UseInventoryItemAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::UseInventoryItem);
	}
}

void ACPlayerCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	// 获取视角摄像机的位置和旋转
	OutLocation = ViewCamera->GetComponentLocation();
	OutRotation = GetBaseAimRotation();
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

void ACPlayerCharacter::LearnAbilityLeaderDown(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("技能按键按下"))
	bIsLearnAbilityLeaderDown = true;
}

void ACPlayerCharacter::LearnAbilityLeaderUp(const FInputActionValue& InputActionValue)
{
	bIsLearnAbilityLeaderDown = false;
}

void ACPlayerCharacter::UseInventoryItem(const FInputActionValue& InputActionValue)
{
	// 获取输入的值,把值传过去
	int Value = FMath::RoundToInt(InputActionValue.Get<float>());
	InventoryComponent->TryActivateItemInSlot(Value-1);
}

void ACPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();

	// 技能升级
	if (bPressed && bIsLearnAbilityLeaderDown)
	{
		// 只会升级Q、E、F、R技能
		if (InputID >= ECAbilityInputID::AbilityQ && InputID <= ECAbilityInputID::AbilityR)
		{
			UpgradeAbilityWithInputID(InputID);
		}
		return;
	}
	
	// 按下
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed(static_cast<int32>(InputID));
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased(static_cast<int32>(InputID));
	}

	// 按下的是普攻键
	if (InputID == ECAbilityInputID::BasicAttack)
	{
		FGameplayTag BasicAttackTag = bPressed ? TGameplayTags::Ability_BasicAttack_Pressed : TGameplayTags::Ability_BasicAttack_Released;
		// 1. 本地直接广播（触发客户端即时反馈）
		// 2. 服务器RPC广播（确保权威状态同步）
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, BasicAttackTag, FGameplayEventData());
		Server_SendGameplayEventToSelf(BasicAttackTag, FGameplayEventData());
	}
}

void ACPlayerCharacter::SetInputEnabledFromPlayerController(bool bEnabled)
{
	// 获取玩家控制器
	APlayerController* PlayerController = GetController<APlayerController>();
	// 如果玩家控制器为空，则返回
	if (!PlayerController)
	{
		return;
	}

	if (bEnabled)
	{
		// 启用玩家控制器输入
		EnableInput(PlayerController);
	}
	else
	{
		// 禁用玩家控制器输入
		DisableInput(PlayerController);
	}
}

/**
 * TODO: 眩晕就禁用一切输入我觉得很抽象，万一有技能被控了也能用呢
 * 或许可以改成眩晕禁用移动的输入
 * APlayerController* PC = GetController<APlayerController>();
 * if (!PC) return;
 * UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PC->InputComponent);
 * if (!EIC) return;
 * // MovementMappingSet 是输入映射集合UInputMappingContext
 * EIC->SetMappingSetEnabled(MovementMappingSet, bEnabled);
 */
void ACPlayerCharacter::OnStun()
{
	SetInputEnabledFromPlayerController(false);
}

void ACPlayerCharacter::OnRecoverFromStun()
{
	if (IsDead()) return;
	
	SetInputEnabledFromPlayerController(true);
}

void ACPlayerCharacter::OnDead()
{
	// 死亡,禁用玩家控制器输入
	SetInputEnabledFromPlayerController(false);
}

void ACPlayerCharacter::OnRespawn()
{
	// 复活,启用玩家控制器输入
	SetInputEnabledFromPlayerController(true);
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

void ACPlayerCharacter::OnAimStateChanged(bool bIsAiming)
{
	// if (IsLocallyControlledByPlayer())
	// 瞄准状态变化时，插值相机到瞄准或默认位置
	LerpCameraToLocalOffsetLocation(bIsAiming ? CameraAimLocalOffset : FVector{0.f});
}

void ACPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{
	GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);

	// 下一帧执行，采取递归的方式
	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(
			this,
			&ACPlayerCharacter::TickCameraLocalOffsetLerp,
			Goal));
}

void ACPlayerCharacter::TickCameraLocalOffsetLerp(FVector Goal)
{
	// 获取相机的位置
	FVector CurrentLocalOffset = ViewCamera->GetRelativeLocation();

	// 如果相机位置与目标位置的距离小于1，则直接设置相机位置
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1.f)
	{
		ViewCamera->SetRelativeLocation(Goal);
		return;
	}
	// 计算插值系数，保证插值平滑且不超过1
	float LerpAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraLerpSpeed, 0.f, 1.f);
	// 执行线性插值计算新位置
	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, LerpAlpha);
	// 设置相机位置
	ViewCamera->SetRelativeLocation(NewLocalOffset);
	// 继续下一帧插值，直到到达目标位置
	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(
			this,
			&ACPlayerCharacter::TickCameraLocalOffsetLerp,
			Goal));
}

