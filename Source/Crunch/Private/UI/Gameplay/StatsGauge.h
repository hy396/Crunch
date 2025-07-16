// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "StatsGauge.generated.h"

/**
 * 绑定属性数值的UI
 */
UCLASS()
class CRUNCH_API UStatsGauge : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构建前
	virtual void NativePreConstruct() override;
	// 构建时
	virtual void NativeConstruct() override;

private:
	// 属性图标控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon;

	// 属性数值文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AttributeText;

	// 需要显示的属性
	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;

	// 图标资源
	UPROPERTY(EditAnywhere, Category = "Visual")
	TObjectPtr<UTexture2D> IconTexture;

	// 设置属性数值显示
	void SetValue(float NewVal);

	// 数字格式化选项
	FNumberFormattingOptions NumberFormattingOptions;

	// 属性变化回调
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
