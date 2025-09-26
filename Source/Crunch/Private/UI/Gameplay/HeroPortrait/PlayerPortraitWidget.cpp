// 幻雨喜欢小猫咪

#include "PlayerPortraitWidget.h"

#include "Character/PDA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h" // 添加进度条头文件
#include "Materials/MaterialInstanceDynamic.h"
#include "Character/CCharacter.h"
#include "GAS/Core/CAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/Core/CHeroAttributeSet.h"

void UPlayerPortraitWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置为整数格式
	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 1;
}

void UPlayerPortraitWidget::SetAscAndPortrait(UAbilitySystemComponent* AbilitySystemComponent,
	const UPDA_CharacterDefinition* CharacterDefinition, bool bIsFriendly, bool bIsSelf)
{
	if (AbilitySystemComponent)
	{
		OwnerAbilitySystemComponent = AbilitySystemComponent;
		// 友方才显示血条和蓝条
		if (bIsFriendly)
		{
			bool bFound;
			float Health = AbilitySystemComponent->GetGameplayAttributeValue(UCAttributeSet::GetHealthAttribute(), bFound);
			float MaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(UCAttributeSet::GetMaxHealthAttribute(), bFound);
			if (bFound)
			{
				SetHealthValue(Health, MaxHealth);
			}
			bFound = false;
			float Mana = AbilitySystemComponent->GetGameplayAttributeValue(UCAttributeSet::GetManaAttribute(), bFound);
			float MaxMana = AbilitySystemComponent->GetGameplayAttributeValue(UCAttributeSet::GetMaxManaAttribute(), bFound);
			if (bFound)
			{
				SetManaValue(Mana, MaxMana);
			}
			// 添加属性值改变委托
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UPlayerPortraitWidget::OnHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UPlayerPortraitWidget::OnMaxHealthChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &UPlayerPortraitWidget::OnManaChanged);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetMaxManaAttribute()).AddUObject(this, &UPlayerPortraitWidget::OnMaxManaChanged);
		}
		// 设置等级
		bool bFound;
		float AttributeValue = AbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
		if (bFound)
		{
			FOnAttributeChangeData OnLevelChangeData;
			OnLevelChangeData.NewValue = AttributeValue;
			SetLevelValue(OnLevelChangeData);
		}
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &UPlayerPortraitWidget::SetLevelValue);

		// 敌方也显示生死
		// 添加死亡标签更新委托
		AbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).AddUObject(this, &UPlayerPortraitWidget::DeathTagUpdated);
		// 开局隐藏死亡倒数计时
		NumberText->SetVisibility(ESlateVisibility::Hidden);
	}
	if (CharacterDefinition)
	{
		// 设置图标
		PortraitImage->GetDynamicMaterial()->SetTextureParameterValue(IconTextureMatParamName, CharacterDefinition->LoadIcon());
		PortraitImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, 1.f);
	}
	if (bIsSelf)
	{
		PortraitBorder->SetBrushColor(FLinearColor(0.5f, 1.0f, 0.5f));
	}else if (bIsFriendly)
	{
		PortraitBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 1.0f, 1.0f));// 蓝色表示友方
	}else
	{
		if (BarBox)
		{
			BarBox->SetWidthOverride(0.f);
			BarBox->SetHeightOverride(0.f);
		}
		// 隐藏血条和蓝条
		HealthBar->SetVisibility(ESlateVisibility::Hidden);
		ManaBar->SetVisibility(ESlateVisibility::Hidden);
		PortraitBorder->SetBrushColor(FLinearColor(0.8f, 0.2f, 0.2f)); // 红色表示敌方
	}
}

void UPlayerPortraitWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	SetHealthValue(Data.NewValue, CacheMaxHealth);
}

void UPlayerPortraitWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	SetHealthValue(CacheHealth, Data.NewValue);
}

void UPlayerPortraitWidget::SetHealthValue(float NewValue, float NewMaxValue)
{
	CacheHealth = NewValue;
	CacheMaxHealth = NewMaxValue;
	// 获取进度条控件并设置百分比
	HealthBar->SetPercent(NewValue / NewMaxValue);
}

void UPlayerPortraitWidget::OnManaChanged(const FOnAttributeChangeData& Data)
{
	SetManaValue(Data.NewValue, CacheMaxMana);
}

void UPlayerPortraitWidget::OnMaxManaChanged(const FOnAttributeChangeData& Data)
{
	SetManaValue(CacheMana, Data.NewValue);
}

void UPlayerPortraitWidget::SetManaValue(float NewValue, float NewMaxValue)
{
	CacheMana = NewValue;
	CacheMaxMana = NewMaxValue;
	// 设置进度条百分比
	ManaBar->SetPercent(NewValue / NewMaxValue);
}

void UPlayerPortraitWidget::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	// 大于0表示死亡，小于0表示复活
	if (NewCount != 0)
	{
		// 缓存的效果总持续时间
		float CachedTotalDuration = 0.f;
		// 获取所有激活的游戏效果
		TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = OwnerAbilitySystemComponent->GetActiveEffects(FGameplayEffectQuery());
		for (const FActiveGameplayEffectHandle& EffectHandle : ActiveEffectHandles)
		{
			// 获取激活的效果
			const FActiveGameplayEffect* ActiveEffect = OwnerAbilitySystemComponent->GetActiveGameplayEffect(EffectHandle);
			if (!ActiveEffect)
			{
				continue;
			}

			// 获取效果标签
			// 在UE 5.3中，InheritableOwnedTagsContainer已被弃用，使用GetGrantedTags()替代
			const FGameplayTagContainer& EffectTags = ActiveEffect->Spec.Def.Get()->GetGrantedTags();

			// 该效果是否有是否标签,若有则break
			bool bIsHaveDeadTagToBreak = false;
			for (const FGameplayTag& Tag_ : EffectTags)
			{
				// 找到死亡标签
				if (Tag_ == TGameplayTags::Stats_Dead)
				{
					// GE
					if (ActiveEffect->Spec.Def)
					{
						ActiveEffect->Spec.Def->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CachedTotalDuration);
						OwnerEffectHandle = ActiveEffect->Handle;
						bIsHaveDeadTagToBreak = true;
						break;
					}
				}
			}
			if (bIsHaveDeadTagToBreak)
			{
				// 启动更新定时器
				if (GetWorld())
				{
					GetWorld()->GetTimerManager().SetTimer(
						UpdateTimerHandle,
						this,
						&UPlayerPortraitWidget::UpdateTimer,
						0.1,
						true,
						0.0f
					);
				}
				break;
			}
		}
		PortraitImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, 0.f);
	}else
	{
		// 死亡Tag移除时
		NumberText->SetVisibility(ESlateVisibility::Hidden);
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
		PortraitImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, 1.f);
	}
}

void UPlayerPortraitWidget::SetLevelValue(const FOnAttributeChangeData& Data)
{
	LevelText->SetText(FText::AsNumber(Data.NewValue, &WholeNumberFormattingOptions));
}

void UPlayerPortraitWidget::UpdateTimer()
{
	// 通过AbilitySystemComponent获取准确的剩余时间
	float RemainingDuration = 0.0f;
	
	// 获取与该效果关联的激活效果
	const FActiveGameplayEffect* ActiveEffect = OwnerAbilitySystemComponent->GetActiveGameplayEffect(OwnerEffectHandle);
	if (ActiveEffect)
	{
		// 使用正确的参数调用GetTimeRemaining函数
		RemainingDuration = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
	}
	// 检查是否为无限持续时间的效果（GetTimeRemaining返回-1.f）
	if (RemainingDuration == -1.f || RemainingDuration <= 0.0f)
	{
		// 对于无限持续时间的效果，隐藏时间显示
		NumberText->SetVisibility(ESlateVisibility::Hidden);
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}
	else
	{
		// 参考AbilityGauge中的时间格式化方式
		// 剩余时间大于1就显示整数，否则显示一位小数
		FNumberFormattingOptions* FormattingOptions = RemainingDuration > 1 ? &WholeNumberFormattingOptions : &TwoDigitNumberFormattingOptions;
		NumberText->SetText(FText::AsNumber(RemainingDuration, FormattingOptions));
		NumberText->SetVisibility(ESlateVisibility::Visible);
	}
}
