// 幻雨喜欢小猫咪

#include "UI/Gameplay/StatusEffect/StatusEffectItemWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "UI/Gameplay/StatusEffect/StatusEffectWidget.h"
#include "AbilitySystemComponent.h"

void UStatusEffectItemWidget::Init(UAbilitySystemComponent* InASC, const FActiveGameplayEffectHandle& InHandle,
	const FStatusEffectData& InData, UStatusEffectWidget* InOwner)
{
	OwnerAsc = InASC;
	OwnerWidget = InOwner;
	EffectHandle = InHandle;

	// 图标
	if (StatusEffectIcon && InData.Icon.IsValid())
	{
		StatusEffectIcon->GetDynamicMaterial()
			->SetTextureParameterValue(
				IconMaterialParamName,
				InData.Icon.LoadSynchronous());
	}

	// 缓存总时长（只用于 UI 百分比）
	CachedTotalDuration =
		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(InHandle);

	// 无限时间 → 隐藏时间文本
	if (CachedTotalDuration < 0.f && DurationText)
	{
		DurationText->SetVisibility(ESlateVisibility::Hidden);
	}

	// 启动 定时器
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&UStatusEffectItemWidget::TickUpdate,
			UpdateInterval,
			true);
	}
}

void UStatusEffectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 设置数字格式化选项，参考AbilityGauge的实现
	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
	
	// 初始化缓存的总时长
	// CachedTotalDuration = 0.0f;
}

void UStatusEffectItemWidget::TickUpdate()
{
	if (!OwnerAsc.IsValid())
	{
		RequestRemove();
		return;
	}

	const FActiveGameplayEffect* ActiveGE =
		OwnerAsc->GetActiveGameplayEffect(EffectHandle);

	// 🔥 唯一“死亡判定”
	if (!ActiveGE)
	{
		RequestRemove();
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const float RemainingTime = ActiveGE->GetTimeRemaining(Now);

	/* ---------- 时间文本 ---------- */

	if (DurationText)
	{
		if (RemainingTime < 0.f)
		{
			DurationText->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			const FNumberFormattingOptions* Format =
				RemainingTime > 1.f
				? &WholeNumberFormattingOptions
				: &TwoDigitNumberFormattingOptions;

			DurationText->SetText(FText::AsNumber(RemainingTime, Format));
			DurationText->SetVisibility(ESlateVisibility::Visible);
		}
	}

	/* ---------- 堆叠 ---------- */

	if (StackCountText)
	{
		// 通过EffectHandle来获取堆叠数上限
		// const int32 StackLimit = ActiveGE->Spec.GetStackLimitCount();
		const int32 StackLimit = UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackLimitCount(EffectHandle);
		const int32 StackCount = ActiveGE->Spec.GetStackCount();

		if (StackLimit > 1 && StackCount > 1)
		{
			StackCountText->SetText(FText::AsNumber(StackCount));
			StackCountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StackCountText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	/* ---------- 材质百分比 ---------- */

	if (StatusEffectIcon && CachedTotalDuration > 0.f && RemainingTime >= 0.f)
	{
		if (UMaterialInstanceDynamic* MID = StatusEffectIcon->GetDynamicMaterial())
		{
			const float Percent = RemainingTime / CachedTotalDuration;
			MID->SetScalarParameterValue(
				RemainingTimePercentParamName,
				1.f - Percent);
		}
	}
}

void UStatusEffectItemWidget::RequestRemove()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}

	if (OwnerWidget.IsValid())
	{
		OwnerWidget->RemoveExpiredStatusEffectItems();
	}

	RemoveFromParent();
}
