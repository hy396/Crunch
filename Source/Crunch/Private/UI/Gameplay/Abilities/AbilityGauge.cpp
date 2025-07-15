// 幻雨喜欢小猫咪


#include "UI/Gameplay/Abilities/AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	// 隐藏冷却计时器
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		// 监听技能释放
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGauge::AbilityCommitted);
	}

	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 2;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	// 获取冷却和消耗
	float CoolDownDuration = UCAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
	float Cost = UCAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

	// 设置冷却和消耗
	CooldownDurationText->SetText(FText::AsNumber(CoolDownDuration));
	CostText->SetText(FText::AsNumber(Cost));
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		// 设置图片
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
	}
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	// 判断释放的技能是否为该图标的技能
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		// 获取技能冷却剩余时长
		float CooldownTimeRemaining = 0.f;
		// 获取技能冷却总时长
		float CooldownDuration = 0.f;
		// 返回当前激活的冷却剩余时间（秒）及该冷却的原始持续时间
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);

		// 获取到的冷却为0，不启动定时器，退出（出现这样的结果可能是没有设置cd的GE，这样启动的定时器会变成负数）
		if (CooldownDuration == 0.f) return;
		// 启动UI技能冷却
		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	// 设置冷却时间
	// CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CooldownCounterText->SetText(FText::AsNumber(CooldownDuration));
	// 缓存冷却总时长
	CachedCooldownDuration = CooldownDuration;
	// 缓存冷却剩余时间
	CachedCooldownTimeRemaining = CooldownTimeRemaining;
	
	// 将倒数计时设置为可视
	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);

	// 冷却结束监听
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UAbilityGauge::CooldownFinished,CachedCooldownTimeRemaining);
	// 启动倒数计时
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdateHandle, this, &UAbilityGauge::UpdateCooldown, CooldownUpdateInterval, true, 0.f);
}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.f;
	// 冷却结束隐藏倒计时文本
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);

	// 关闭倒数计时定时器
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f);
}

void UAbilityGauge::UpdateCooldown()
{
	// 更新剩余时间
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	// 剩余时间大于1就显示一位数
	FNumberFormattingOptions* FormattingOptions = CachedCooldownTimeRemaining > 1 ? &WholeNumberFormattingOptions : &TwoDigitNumberFormattingOptions;
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.0f - CachedCooldownTimeRemaining / CachedCooldownDuration);
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, FormattingOptions));

	// if (CachedCooldownTimeRemaining <= 0.f)
	// {
	// 	CooldownFinished();
	// }
}
