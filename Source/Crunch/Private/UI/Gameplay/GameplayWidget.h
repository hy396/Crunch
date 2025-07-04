// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ValueGauge.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 当Widget被创建并添加到视口时，此函数会被自动调用
	virtual void NativeConstruct() override;
private:
	// 生命进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthBar;

	// 法力进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaBar;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
};
