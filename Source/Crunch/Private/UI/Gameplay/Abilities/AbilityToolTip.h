// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityToolTip.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityToolTip : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityTexture, const FText& AbilityDescription, float AbilityCooldown, float AbilityCost);

	void SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityTexture, const FText& AbilityDescription, const FText& AbilityCooldown, const FText& AbilityCost);

private:
	// 技能名称
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UTextBlock> AbilityNameText;

	// 技能图标
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UImage> AbilityIcon;

	// 技能描述
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UTextBlock> AbilityDescriptionText;

	// 技能冷却
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UTextBlock> AbilityCooldownText;

	// 技能消耗
	UPROPERTY(meta=(BindWidget))	
	TObjectPtr<UTextBlock> AbilityCostText;
};
