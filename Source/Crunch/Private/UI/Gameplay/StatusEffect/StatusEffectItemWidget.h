// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"  // 确保包含以使用 FGameplayEffectRemovalInfo
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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
	void Init(
		UAbilitySystemComponent* InASC,
		const FActiveGameplayEffectHandle& InHandle,
		const FStatusEffectData& InData,
		UStatusEffectWidget* InOwner);
	virtual void NativeConstruct() override;
	/** 销毁时自动清理（防止内存泄漏） */
	virtual void NativeDestruct() override;

// private:
	// ========== UI组件 ==========
	// 状态效果图标
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> StatusEffectIcon;

	// 剩余时间文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DurationText;

	// 堆叠数文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StackCountText;
	
	// ========== 配置参数 ==========
	// 更新间隔（秒）
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect")
	float UpdateInterval = 0.2f;

	// 材质参数名 - 剩余时间百分比
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName RemainingTimePercentParamName = "Percent";

	// 图标材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";

	// ========== 运行时状态 ==========

	// 效果句柄
	FActiveGameplayEffectHandle EffectHandle;

	// 缓存的效果总持续时间
	float CachedTotalDuration = 0.f;
	// AbilitySystemComponent引用，用于获取准确的剩余时间
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAsc;
	TWeakObjectPtr<UStatusEffectWidget> OwnerWidget;

	// 数字格式化选项（整数），参考AbilityGauge的实现
	FNumberFormattingOptions WholeNumberFormattingOptions;

	// 数字格式化选项（一位小数），参考AbilityGauge的实现
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;
	// 设置时间
	void SetDurationTextSafe(float RemainingTime);
	void RefreshStackDisplay();


	FTimerHandle UpdateTimerHandle;
	void StartUpdateTimer();      // 启动定时器
	void StopUpdateTimer();       // 停止定时器
	void UpdateVisuals_v2();

	// ========== 回调函数 ==========
	
	/** 时间变化事件（总时长修改、刷新、网络同步时触发） */
	void OnEffectTimeChanged(FActiveGameplayEffectHandle Handle, float StartTime, float Duration);
	
	/** 堆叠数变化事件（加层/减层时触发） */
	void OnStackChanged(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 OldCount);
	
	/** GE被移除事件（GE结束时触发，用于立即清理UI） */
	void OnEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo);
	
	/** 原本绑定在定时器的更新函数，现已移除*/
	void TickUpdate();

	
	/** 从父控件移除并清理自身 */
	void RequestRemove();

	// 修改为：统一清理入口
	void CleanupAndRemove();

	// 运行时状态
	bool bIsCleaningUp = false;
};