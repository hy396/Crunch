// 幻雨喜欢小猫咪

#include "UI/Gameplay/StatusEffect/StatusEffectItemWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "UI/Gameplay/StatusEffect/StatusEffectWidget.h"
#include "AbilitySystemComponent.h"

// void UStatusEffectItemWidget::Init(UAbilitySystemComponent* InASC, const FActiveGameplayEffectHandle& InHandle,
// 	const FStatusEffectData& InData, UStatusEffectWidget* InOwner)
// {
// 	OwnerAsc = InASC;
// 	OwnerWidget = InOwner;
// 	EffectHandle = InHandle;
//
// 	// 图标
// 	if (StatusEffectIcon && InData.Icon.IsValid())
// 	{
// 		StatusEffectIcon->GetDynamicMaterial()
// 			->SetTextureParameterValue(
// 				IconMaterialParamName,
// 				InData.Icon.LoadSynchronous());
// 	}
//
// 	// 缓存总时长（只用于 UI 百分比）
// 	CachedTotalDuration =
// 		UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(InHandle);
//
// 	// 无限时间 → 隐藏时间文本
// 	if (CachedTotalDuration < 0.f && DurationText)
// 	{
// 		DurationText->SetVisibility(ESlateVisibility::Hidden);
// 	}
//
// 	// 启动 定时器
// 	if (GetWorld())
// 	{
// 		GetWorld()->GetTimerManager().SetTimer(
// 			UpdateTimerHandle,
// 			this,
// 			&UStatusEffectItemWidget::TickUpdate,
// 			UpdateInterval,
// 			true);
// 	}
// }
//
void UStatusEffectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 设置数字格式化选项，参考AbilityGauge的实现
	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
	
	// 初始化缓存的总时长
	// CachedTotalDuration = 0.0f;
}

void UStatusEffectItemWidget::NativeDestruct()
{
	// 确保在Widget销毁时清理所有委托绑定（双重保险）
	RequestRemove();
	Super::NativeDestruct();
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

	SetDurationTextSafe(RemainingTime);

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

void UStatusEffectItemWidget::SetDurationTextSafe(float RemainingTime)
{
	if (!DurationText)
	{
		return;
	}

	// 无限 / 已失效
	if (RemainingTime < 0.f)
	{
		DurationText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	FNumberFormattingOptions* FormattingOptions = RemainingTime > 1 ? &WholeNumberFormattingOptions : &TwoDigitNumberFormattingOptions;
	DurationText->SetText(FText::AsNumber(RemainingTime, FormattingOptions));
	DurationText->SetVisibility(ESlateVisibility::Visible);
}

void UStatusEffectItemWidget::RefreshStackDisplay()
{
	if (!StackCountText || !OwnerAsc.IsValid())
	{
		return;
	}

	const FActiveGameplayEffect* ActiveGE =
		OwnerAsc->GetActiveGameplayEffect(EffectHandle);

	if (!ActiveGE)
	{
		StackCountText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const int32 StackLimit =
		ActiveGE->Spec.Def ? ActiveGE->Spec.Def->GetStackLimitCount() : 0;

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


void UStatusEffectItemWidget::StartUpdateTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&UStatusEffectItemWidget::UpdateVisuals_v2,  // 定时刷新
			UpdateInterval,
			true);  // 循环执行
	}
}

void UStatusEffectItemWidget::StopUpdateTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}
}

void UStatusEffectItemWidget::UpdateVisuals_v2()
{
	if (!OwnerAsc.IsValid()) 
	{
		RequestRemove();
		return;
	}

	const FActiveGameplayEffect* ActiveGE = OwnerAsc->GetActiveGameplayEffect(EffectHandle);
    
	// 死亡判定
	if (!ActiveGE || ActiveGE->IsPendingRemove)
	{
		RequestRemove();
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const float RemainingTime = ActiveGE->GetTimeRemaining(Now);

	/* ---------- 时间文本 ---------- */
	SetDurationTextSafe(RemainingTime);
	// if (StackCountText)
	// {
	// 	// 通过EffectHandle来获取堆叠数上限
	// 	// const int32 StackLimit = ActiveGE->Spec.GetStackLimitCount();
	// 	const int32 StackLimit = UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectStackLimitCount(EffectHandle);
	// 	const int32 StackCount = ActiveGE->Spec.GetStackCount();
	//
	// 	if (StackLimit > 1 && StackCount > 1)
	// 	{
	// 		StackCountText->SetText(FText::AsNumber(StackCount));
	// 		StackCountText->SetVisibility(ESlateVisibility::Visible);
	// 	}
	// 	else
	// 	{
	// 		StackCountText->SetVisibility(ESlateVisibility::Collapsed);
	// 	}
	// }

	/* ---------- 材质进度条 ---------- */
	if (StatusEffectIcon && CachedTotalDuration > 0.f && RemainingTime >= 0.f)
	{
		if (UMaterialInstanceDynamic* MID = StatusEffectIcon->GetDynamicMaterial())
		{
			const float Percent = FMath::Clamp(RemainingTime / CachedTotalDuration, 0.f, 1.f);
			MID->SetScalarParameterValue(RemainingTimePercentParamName, 1.f - Percent);
		}
	}
}

//
// void UStatusEffectItemWidget::RequestRemove()
// {
// 	if (GetWorld())
// 	{
// 		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
// 	}
//
// 	if (OwnerWidget.IsValid())
// 	{
// 		OwnerWidget->RemoveExpiredStatusEffectItems();
// 	}
//
// 	RemoveFromParent();
// }

void UStatusEffectItemWidget::Init(UAbilitySystemComponent* InASC, const FActiveGameplayEffectHandle& InHandle,
    const FStatusEffectData& InData, UStatusEffectWidget* InOwner)
{
	OwnerAsc = InASC;
	OwnerWidget = InOwner;
	EffectHandle = InHandle;
	// if (DurationText)
	// {
	// 	DurationText->SetVisibility(ESlateVisibility::Hidden);
	// }
    // 图标设置保持不变...
	// 不用InData.Icon.IsValid()来判断， IsValid是加载到了内存中， IsNULL是路径为空（DataTable 里没填）
	if (StatusEffectIcon && !InData.Icon.IsNull())
	{
		// UE_LOG(LogTemp, Warning, TEXT("设置图标"))
		StatusEffectIcon->GetDynamicMaterial()
			->SetTextureParameterValue(
				IconMaterialParamName,
				InData.Icon.LoadSynchronous());
	}	
    // 🔥 关键：直接找到 FActiveGameplayEffect 并绑定事件（只做一次查找）
    if (FActiveGameplayEffect* ActiveGE = const_cast<FActiveGameplayEffect*>(OwnerAsc->GetActiveGameplayEffect(EffectHandle)))
    {
        // 绑定时间变化事件（包括持续时间修改、堆叠刷新时间等）
        ActiveGE->EventSet.OnTimeChanged.AddUObject(this, &UStatusEffectItemWidget::OnEffectTimeChanged);
        
        // 绑定堆叠变化事件
        ActiveGE->EventSet.OnStackChanged.AddUObject(this, &UStatusEffectItemWidget::OnStackChanged);
        
        // 绑定移除事件（GE消失时自动移除UI）
        ActiveGE->EventSet.OnEffectRemoved.AddUObject(this, &UStatusEffectItemWidget::OnEffectRemoved);
        
        // 初始化显示（手动调一次，因为绑定事件时可能已经过了触发时机）
        // OnEffectTimeChanged(EffectHandle, ActiveGE->StartWorldTime, ActiveGE->GetDuration());
        // OnStackChanged(EffectHandle, ActiveGE->Spec.GetStackCount(), 0);
        
        // 缓存总时长（通过事件会自动更新，不需要手动缓存）
        // CachedTotalDuration = ActiveGE->GetDuration();

    	CachedTotalDuration = UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(InHandle);
    	// UpdateVisuals_v2();
    	// 🔥 关键：启动定时器（处理时间自然流逝）
    	if (CachedTotalDuration > 0)
    	{
    		StartUpdateTimer();
    	}
    	// 初始化完成后，显式刷新 UI
    	RefreshStackDisplay();
    	// UpdateVisuals_v2();

        // // 无限时间检查
        // if (CachedTotalDuration < 0.f && DurationText)
        // {
        //     DurationText->SetVisibility(ESlateVisibility::Hidden);
        // }
    }
    else
    {
        // GE 已经没了，直接移除
        RequestRemove();
    }
}

// 时间变化时自动调用（网络同步、刷新时间、修改时间都会触发）
void UStatusEffectItemWidget::OnEffectTimeChanged(FActiveGameplayEffectHandle Handle, float StartTime, float Duration)
{
	CachedTotalDuration = Duration;  // 更新缓存的总时长
    
	// 🔥 如果之前是无限时间，现在变成有限，要启动 Timer
	if (Duration > 0 && !GetWorld()->GetTimerManager().IsTimerActive(UpdateTimerHandle))
	{
		StartUpdateTimer();
	}
	// 如果变成无限时间，停止 Timer
	else if (Duration < 0)
	{
		StopUpdateTimer();
		// if (DurationText) DurationText->SetVisibility(ESlateVisibility::Hidden);
	}
	UpdateVisuals_v2();
}

// 堆叠变化时自动调用（加层、减层都会触发）
void UStatusEffectItemWidget::OnStackChanged(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 OldCount)
{
	// if (!StackCountText || !OwnerAsc.IsValid()) return;
	//
	// // 更新堆叠显示...
 //    if (const FActiveGameplayEffect* ActiveGE = OwnerAsc->GetActiveGameplayEffect(EffectHandle))
 //    {
 //        const int32 StackLimit = ActiveGE->Spec.Def ? ActiveGE->Spec.Def->GetStackLimitCount() : 0;
 //        
 //        if (StackLimit > 1 && NewCount > 1)
 //        {
 //            StackCountText->SetText(FText::AsNumber(NewCount));
 //            StackCountText->SetVisibility(ESlateVisibility::Visible);
 //        }
 //        else
 //        {
 //            StackCountText->SetVisibility(ESlateVisibility::Collapsed);
 //        }
 //    }
	RefreshStackDisplay();
	// 🔥 堆叠变化通常伴随 Duration Refresh，立即刷新时间显示
	UpdateVisuals_v2();
}

// GE被移除时自动调用
void UStatusEffectItemWidget::OnEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	CleanupAndRemove();
}

void UStatusEffectItemWidget::RequestRemove()
{
	CleanupAndRemove();
}

void UStatusEffectItemWidget::CleanupAndRemove()
{
	// 🔥 防止重复执行（如果被多次调用）
	if (bIsCleaningUp) return;
	bIsCleaningUp = true;
	
	StopUpdateTimer();  // 停止定时器
    
	// 2. 解绑事件（此时 GE 还存在，因为 OnEffectRemoved 是移除前回调）
	if (OwnerAsc.IsValid())
	{
		if (FActiveGameplayEffect* ActiveGE = const_cast<FActiveGameplayEffect*>(
			OwnerAsc->GetActiveGameplayEffect(EffectHandle)))
		{
			ActiveGE->EventSet.OnTimeChanged.RemoveAll(this);
			ActiveGE->EventSet.OnStackChanged.RemoveAll(this);
			ActiveGE->EventSet.OnEffectRemoved.RemoveAll(this);
		}
	}
	// 通知父级...
	if (OwnerWidget.IsValid())
	{
		OwnerWidget->RemoveStatusEffectItem(EffectHandle);
	}
	// 4. 从 UI 树移除
	RemoveFromParent();
}
