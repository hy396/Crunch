// 幻雨喜欢小猫咪

#include "PlayerPortraitWidget.h"

#include "Character/PDA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h" // 添加进度条头文件
#include "Materials/MaterialInstanceDynamic.h"
#include "Character/CCharacter.h"
#include "GAS/Core/CAttributeSet.h"
#include "AbilitySystemComponent.h"

void UPlayerPortraitWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerPortraitWidget::SetAscAndPortrait(UAbilitySystemComponent* AbilitySystemComponent,
	const UPDA_CharacterDefinition* CharacterDefinition, bool bIsFriendly, bool bIsSelf)
{
	if (AbilitySystemComponent)
	{
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
		
		// 敌方也显示生死
		// 添加死亡标签更新委托
		AbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Dead).AddUObject(this, &UPlayerPortraitWidget::DeathTagUpdated);
	}
	if (CharacterDefinition)
	{
		// 设置图标
		PortraitImage->GetDynamicMaterial()->SetTextureParameterValue(IconTextureMatParamName, CharacterDefinition->LoadIcon());
		PortraitImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, 0.f);
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
	if (NewCount > 0)
	{
		PortraitImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, 1.f);
	}else
	{
		PortraitImage->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, 0.f);
	}
}
