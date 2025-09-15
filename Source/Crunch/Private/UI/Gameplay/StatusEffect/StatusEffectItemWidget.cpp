// 幻雨喜欢小猫咪

#include "UI/Gameplay/StatusEffect/StatusEffectItemWidget.h"
#include "GameplayEffectTypes.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "UI/Gameplay/StatusEffect/StatusEffectWidget.h"
#include "AbilitySystemComponent.h"

void UStatusEffectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 设置数字格式化选项，参考AbilityGauge的实现
	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
	
	// 初始化缓存的总时长
	// CachedTotalDuration = 0.0f;
}

void UStatusEffectItemWidget::InitializeStatusEffectItem(const FActiveGameplayEffect& ActiveEffect, UStatusEffectWidget* StatusEffectWidget)
{
	AssociatedGameplayEffect = ActiveEffect.Spec.Def;
	EffectHandle = ActiveEffect.Handle;

	// 获取效果标签
	// 在UE 5.3中，InheritableOwnedTagsContainer已被弃用，使用GetGrantedTags()替代
	const FGameplayTagContainer& EffectTags = ActiveEffect.Spec.Def.Get()->GetGrantedTags();
	// 通过数据表查找图标
	const FStatusEffectData* StatusEffectData = FindStatusEffectData(EffectTags, StatusEffectWidget->GetStatusEffectDataTable());
	if (StatusEffectData && StatusEffectIcon)
	{
		// 设置图标，参考AbilityGauge的实现方式
		StatusEffectIcon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, StatusEffectData->Icon.LoadSynchronous());
	}
	else if (StatusEffectIcon)
	{
		// 如果没有找到对应的图标，使用默认图标
		// 可以设置一个默认的状态效果图标
		// StatusEffectIcon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, DefaultStatusEffectIcon.LoadSynchronous());
	}

	// 设置堆叠数（如果有的话）
	if (StackCountText)
	{
		int32 StackCount = ActiveEffect.Spec.GetStackCount();
		if (StackCount > 1)
		{
			StackCountText->SetText(FText::AsNumber(StackCount));
			StackCountText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StackCountText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 保存OwnerASC引用用于后续获取准确的剩余时间
	OwnerASC = StatusEffectWidget->GetOwnerASC();

	// 缓存效果的总持续时间
	if (AssociatedGameplayEffect)
	{
		// 使用DurationMagnitude的GetStaticMagnitudeIfPossible方法获取持续时间
		AssociatedGameplayEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CachedTotalDuration);
	}
	
	// 存储持续时间
	// CachedTotalDuration = ActiveEffect.GetTimeRemaining(GetWorld()->GetTimeSeconds());

	// 启动更新定时器
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			UpdateTimerHandle,
			this,
			&UStatusEffectItemWidget::UpdateTimer,
			UpdateInterval,
			true,
			0.0f
		);
	}

	// 初始更新
	UpdateTimer();
}

void UStatusEffectItemWidget::UpdateDisplay(float RemainingDuration)
{
	if (DurationText)
	{
		// 检查是否为无限持续时间的效果（GetTimeRemaining返回-1.f）
		if (RemainingDuration == -1.f || RemainingDuration <= 0.0f)
		{
			// 对于无限持续时间的效果，隐藏时间显示
			DurationText->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			// 参考AbilityGauge中的时间格式化方式
			// 剩余时间大于1就显示整数，否则显示一位小数
			FNumberFormattingOptions* FormattingOptions = RemainingDuration > 1 ? &WholeNumberFormattingOptions : &TwoDigitNumberFormattingOptions;
			DurationText->SetText(FText::AsNumber(RemainingDuration, FormattingOptions));
			DurationText->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// 更新材质中的剩余时间百分比参数
	if (StatusEffectIcon)
	{
		// 使用缓存的总持续时间计算剩余时间百分比
		float RemainingPercent = 1.0f;
		if (CachedTotalDuration > 0.0f && RemainingDuration >= 0.0f)
		{
			RemainingPercent = RemainingDuration / CachedTotalDuration;
		}
		// 更新材质参数
		if (StatusEffectIcon->GetDynamicMaterial())
		{
			StatusEffectIcon->GetDynamicMaterial()->SetScalarParameterValue(RemainingTimePercentParamName, 1 - RemainingPercent);
		}
	}
}

void UStatusEffectItemWidget::UpdateTimer()
{
	if (!AssociatedGameplayEffect || !OwnerASC)
	{
		return;
	}

	// 通过AbilitySystemComponent获取准确的剩余时间
	float RemainingDuration = 0.0f;
	
	// 获取与该效果关联的激活效果
	const FActiveGameplayEffect* ActiveEffect = OwnerASC->GetActiveGameplayEffect(EffectHandle);
	if (ActiveEffect)
	{
		// 使用正确的参数调用GetTimeRemaining函数
		RemainingDuration = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		
		// 检查是否为瞬时效果（INSTANT_APPLICATION）
		if (ActiveEffect->GetDuration() == FGameplayEffectConstants::INSTANT_APPLICATION)
		{
			// 对于瞬时效果，我们可以选择立即移除UI或者显示特殊标记
			// 这里我们选择不显示时间
			RemainingDuration = -1.0f;
		}
	}
	if (RemainingDuration == 0.0f || RemainingDuration == -1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}
	UpdateDisplay(RemainingDuration);
}

// 静态方法实现：通过标签查找状态效果数据
const FStatusEffectData* UStatusEffectItemWidget::FindStatusEffectData(const FGameplayTagContainer& Tags, UDataTable* DataTable)
{
	if (!DataTable)
	{
		return nullptr;
	}

	// 遍历所有标签，查找匹配的数据表行
	for (const FGameplayTag& Tag : Tags)
	{
		// 遍历数据表的所有行
		for (auto& StatusEffectDataPair : DataTable->GetRowMap())
		{
			// 查找当前行的数据
			const FStatusEffectData* WidgetData = DataTable->FindRow<FStatusEffectData>(StatusEffectDataPair.Key, "");
        
			// 如果找到数据且技能类匹配则返回
			if (WidgetData && WidgetData->StatusEffectTag == Tag)
			{
				return WidgetData;
			}
		}
	}

	// 如果没有精确匹配，直接返回nullptr
	return nullptr;
}
