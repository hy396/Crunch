// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "GameplayEffectTypes.h"
#include "Components/TextBlock.h"
#include "ValueBar.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UValueBar : public UUserWidget
{
	GENERATED_BODY()
public:
	// 构造函数
	virtual void NativeConstruct() override;
	void SetDynamicMaterials();
	void InitializeBarVisuals();
	void ResetAnimatedState();
	
	/**
	 * @brief 绑定到游戏属性并设置当前值与最大值
	 * 
	 * 将该数值指示器绑定到指定的能力系统组件中的游戏属性，
	 * 同时也绑定到对应的最大值属性。当属性值发生变化时，
	 * 指示器会自动更新显示。
	 *
	 * @param AbilitySystemComponent 能力系统组件，用于注册和监听属性变化
	 * @param Attribute 当前值的游戏属性
	 * @param MaxAttribute 最大值的游戏属性
	 */	
	void SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);

	void SetValue(float NewValue, float NewMaxValue);

	// 设置每秒回复值
	void SetRegenValueTextToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayAttribute& Attribute);
	void SetRegenValue(float NewRegenValue);
protected:
	// // 绑定的属性
	// UPROPERTY(EditAnywhere, Category = "Attribute")
	// FGameplayAttribute Attribute;
	
	// 数字边框
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NumberBorder;

	// 基础数值
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCommonNumericTextBlock> CurrentValueNumber;

	// 最大数值
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCommonNumericTextBlock> MaxValueNumber;

	// 放置右边显示每秒回复的数值
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> RegenValueText;

	// 进度条边框
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> BarBorder;

	// 进度条
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> BarFill;

	// 
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UImage> BarGlow;
	// 受伤动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OnDamaged;

	// 回复动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OnHealed;

	// 死亡动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OnEliminated;

	// 创建动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> OnSpawned;

private:
	// 材质动态实例
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BarBorderMID = nullptr;
	// 材质动态实例
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BarFillMID = nullptr;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BarGlowMID = nullptr;
	float NormalizedHealth = 0.f;
	float OldValue = 0.f;
	float NewValue = 0.f;

	// 缓存属性
	float CachedValue = 0.f;
	float CachedMaxValue = 0.f;
	
	// 回调用函数
	void ValueChanged(const FOnAttributeChangeData& ChangeData);
	void MaxValueChanged(const FOnAttributeChangeData& ChangeData);

	void RegenValueChanged(const FOnAttributeChangeData& ChangeData);
};
