// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Components/WidgetComponent.h"
#include "GAS/Core/CAbilitySystemComponent.h"
#include "GAS/Core/CAttributeSet.h"
#include "CCharacter.generated.h"

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface
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
private:
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
};
