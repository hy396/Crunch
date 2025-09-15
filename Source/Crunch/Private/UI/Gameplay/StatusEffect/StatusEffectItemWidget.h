// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameplayEffectTypes.h"
#include "StatusEffectWidget.h"
#include "StatusEffectItemWidget.generated.h"

class UDataTable;
class UStatusEffectWidget;
class UAbilitySystemComponent;

/**
 * 单个状态效果项目UI控件，用于显示一个状态效果的图标、持续时间和堆叠数
 */
UCLASS()
class CRUNCH_API UStatusEffectItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 初始化状态效果项目显示
	void InitializeStatusEffectItem(const FActiveGameplayEffect& ActiveEffect, UStatusEffectWidget* StatusEffectWidget);

	// 更新状态效果显示（持续时间等）
	void UpdateDisplay(float RemainingDuration);

	// 获取关联的GameplayEffect
	FORCEINLINE const UGameplayEffect* GetGameplayEffect() const { return AssociatedGameplayEffect; }

	// 通过标签查找状态效果数据（静态方法，可在其他类中复用）
	static const FStatusEffectData* FindStatusEffectData(const FGameplayTagContainer& Tags, UDataTable* DataTable);

private:
	// 状态效果图标
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> StatusEffectIcon;

	// 剩余时间文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DurationText;

	// 堆叠数文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StackCountText;

	// 关联的GameplayEffect
	UPROPERTY()
	const UGameplayEffect* AssociatedGameplayEffect;

	// 效果句柄
	FActiveGameplayEffectHandle EffectHandle;

	// 更新定时器
	FTimerHandle UpdateTimerHandle;

	// 更新间隔（秒）
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect")
	float UpdateInterval = 0.1f;

	// AbilitySystemComponent引用，用于获取准确的剩余时间
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	// 数字格式化选项（整数），参考AbilityGauge的实现
	FNumberFormattingOptions WholeNumberFormattingOptions;

	// 数字格式化选项（一位小数），参考AbilityGauge的实现
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;

	// 材质参数名 - 剩余时间百分比
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName RemainingTimePercentParamName = "Percent";

	// 图标材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";

	// 缓存的效果总持续时间
	float CachedTotalDuration = 0.f;

	// 更新显示
	void UpdateTimer();

	// 通过标签查找状态效果数据
	// const FStatusEffectData* FindStatusEffectData(const FGameplayTagContainer& Tags, UDataTable* DataTable) const;
};