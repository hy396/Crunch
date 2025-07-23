// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/Core/CAttributeSet.h"
#include "LevelGauge.generated.h"

/**
 * ULevelGauge
 * 用于显示角色等级进度的UI控件（包含进度条和文本显示）
 * 依赖GameplayAbilities系统实现属性绑定
 */
UCLASS()
class CRUNCH_API ULevelGauge : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	/**
	 * 材质参数名称（用于动态调整进度条材质）
	 * 在编辑器中可配置，默认值为"Percent"
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName PercentMaterialParamName = "Percent";

	// 等级进度条
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LevelProgressImage;

	// 等级文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;

	// 数字格式化设置
	FNumberFormattingOptions NumberFormattingOptions;
	
	TObjectPtr<const UAbilitySystemComponent> OwnerASC;

	void UpdateGauge(const FOnAttributeChangeData& Data);
};
