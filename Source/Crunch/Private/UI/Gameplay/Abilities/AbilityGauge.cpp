// 幻雨喜欢小猫咪


#include "UI/Gameplay/Abilities/AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
#include "GAS/Core/CAttributeSet.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "GAS/Core/CHeroAttributeSet.h"

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

		// 监听技能规格更新
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UAbilityGauge::AbilitySpecUpdated);

		// 绑定升级点属性变化事件 - 当玩家获得/消耗升级点时触发
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetUpgradePointAttribute())
			.AddUObject(this, &UAbilityGauge::UpgradePointUpdated);
        
		// 绑定法力值属性变化事件 - 当玩家法力值变化时触发
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute())
			.AddUObject(this, &UAbilityGauge::ManaUpdated);

		// 初始化升级点显示（获取当前值并刷新UI）
        bool bFound = false;
        float UpgradePoint = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetUpgradePointAttribute(), bFound);
        if (bFound)
        {
            // 创建属性变化数据结构（模拟属性变化事件）
            FOnAttributeChangeData ChangeData;
            ChangeData.NewValue = UpgradePoint;
            
            // 手动调用升级点更新函数以刷新UI
            UpgradePointUpdated(ChangeData);
        }

		// 保存能力系统组件引用供后续使用
		OwnerAbilitySystemComponent = OwnerASC;
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
	// 初始化技能等级
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0);
	// 初始化技能的最大等级
	// 获取当前技能规格
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (AbilitySpec)
	{
		float MaxLevel = AbilitySpec->InputID == static_cast<int32>(ECAbilityInputID::AbilityR) ? 3 : 5;
		LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(MaxLevelParamName, MaxLevel);
	}
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

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	// 技能组件和技能对象不存在
	if (!OwnerAbilitySystemComponent || !AbilityCDO) return nullptr;

	// 技能缓存句柄无效，重新查找
	if (!CachedAbilitySpecHandle.IsValid())
	{
		// 根据技能类查找规格
		FGameplayAbilitySpec* FoundAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass());
        
		// 缓存找到的规格句柄（Handle）
		CachedAbilitySpecHandle = FoundAbilitySpec->Handle;
		return FoundAbilitySpec;
	}

	// 技能缓存句柄有效，返回缓存的规格
	return OwnerAbilitySystemComponent->FindAbilitySpecFromHandle(CachedAbilitySpecHandle);
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	// 检测技能是否为该图标技能
	if (AbilitySpec.Ability != AbilityCDO) return;

	// 更新学习状态
	bIsAbilityLearned = AbilitySpec.Level > 0;

	// 更新显示的技能等级
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, AbilitySpec.Level);

	// 刷新技能能否释放的状态
	UpdateCanCast();

	// 并显示新的冷却时间和法力消耗
	float NewCooldownDuration = UCAbilitySystemStatics::GetCooldownDurationFor(AbilitySpec.Ability, *OwnerAbilitySystemComponent, AbilitySpec.Level);
	float NewCost = UCAbilitySystemStatics::GetManaCostFor(AbilitySpec.Ability, *OwnerAbilitySystemComponent, AbilitySpec.Level);
	CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdateCanCast()
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	
	// 技能学习才能亮起来
	bool bCanCast = bIsAbilityLearned;
	
	// 看蓝量是否够
	if (AbilitySpec && OwnerAbilitySystemComponent)
	{
		if (!UCAbilitySystemStatics::CheckAbilityCost(*AbilitySpec, *OwnerAbilitySystemComponent))
		{
			bCanCast = false;
		}
	}
	// 更新UI材质显示（1=可释放，0=不可释放）
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bCanCast ? 1 : 0);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& Data)
{
	// 检查是否有可用升级点
	bool HasUpgradePoint = Data.NewValue > 0;
	// 获取当前技能规格
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
    
	if (AbilitySpec && OwnerAbilitySystemComponent)
	{
		// 获取玩家等级
		bool bFound;
		float CurrentLevel = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
		if (bFound)
		{
			// 如果技能已达目前的最大等级，不显示升级提示
			if (UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec, CurrentLevel))
			{
				Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, 0);
				return;
			}
		}
	}
    
	// 更新UI材质显示（1=可升级，0=不可升级）
	Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvailableParamName, HasUpgradePoint ? 1 : 0);
}

void UAbilityGauge::ManaUpdated(const FOnAttributeChangeData& Data)
{
	UpdateCanCast();
}
