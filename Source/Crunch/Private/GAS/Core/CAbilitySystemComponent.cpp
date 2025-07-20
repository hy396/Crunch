// 幻雨喜欢小猫咪


#include "GAS/Core/CAbilitySystemComponent.h"

#include "CAttributeSet.h"
#include "CHeroAttributeSet.h"
#include "TGameplayTags.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated);
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &UCAbilitySystemComponent::ManaUpdated);

	GenericConfirmInputID = static_cast<int32>(ECAbilityInputID::Confirm);
	GenericCancelInputID = static_cast<int32>(ECAbilityInputID::Cancel);
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!BaseStatDataTable || !GetOwner())
	{
		return;
	}

	// const FHeroBaseStats* BaseStats = nullptr;
	//
	// for (const TPair<FName, uint8*>& DataPair : BaseStatDataTable->GetRowMap())
	// {
	// 	BaseStats = BaseStatDataTable->FindRow<FHeroBaseStats>(DataPair.Key, "");
	// 	if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
	// 	{
	// 		break; // 找到后退出循环
	// 	}
	// }
	//
	// if (BaseStats)
	// {
	// 	// 设置基础战斗属性
	// 	SetNumericAttributeBase(UCAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);			// 最大生命值
	// 	SetNumericAttributeBase(UCAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);				// 最大魔法值
	// 	SetNumericAttributeBase(UCAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);	// 攻击伤害
	// 	SetNumericAttributeBase(UCAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);					// 护甲值
	// 	SetNumericAttributeBase(UCAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);			// 移动速度
	//
	// 	// 设置角色成长属性
	// 	SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);								// 力量
	// 	SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);			// 力量成长率
	// 	SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);						// 智力
	// 	SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);	// 智力成长率
	// }

	const FTHeroBaseStats* BaseStats = nullptr;

	for (const TPair<FName, uint8*>& DataPair : BaseStatDataTable->GetRowMap())
	{
		BaseStats = BaseStatDataTable->FindRow<FTHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
		{
			break; // 找到后退出循环
		}
	}
	
	if (BaseStats)
	{
		// 设置基础战斗属性
		SetNumericAttributeBase(UCAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);				// 最大生命值
		SetNumericAttributeBase(UCAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);					// 最大魔法值
		SetNumericAttributeBase(UCAttributeSet::GetAttackPowerAttribute(), BaseStats->AttackPower);				// 攻击力
		SetNumericAttributeBase(UCAttributeSet::GetMagicPowerAttribute(), BaseStats->MagicPower);				// 法术强度
		SetNumericAttributeBase(UCAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);						// 护甲值
		SetNumericAttributeBase(UCAttributeSet::GetMagicResistanceAttribute(), BaseStats->BaseMagicResistance);	// 法术抗性
		SetNumericAttributeBase(UCAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);				// 移动速度

		SetNumericAttributeBase(UCHeroAttributeSet::GetHealthRegenAttribute(), BaseStats->HealthRegen);			// 生命回复
		SetNumericAttributeBase(UCHeroAttributeSet::GetManaRegenAttribute(), BaseStats->ManaRegen);				// 法术回复
		// 设置角色成长属性
		SetNumericAttributeBase(UCHeroAttributeSet::GetAttackPowerGrowthRateAttribute(), BaseStats->AttackPowerGrowthRate);	// 攻击成长率
		SetNumericAttributeBase(UCHeroAttributeSet::GetMagicPowerGrowthRateAttribute(), BaseStats->MagicPowerGrowthRate);	// 法术成长率
		SetNumericAttributeBase(UCHeroAttributeSet::GetHealthRegenGrowthRateAttribute(), BaseStats->AttackPowerGrowthRate);	// 生命回复成长
		SetNumericAttributeBase(UCHeroAttributeSet::GetManaRegenGrowthRateAttribute(), BaseStats->MagicPowerGrowthRate);	// 法力回复成长
		
	}
	
}

void UCAbilitySystemComponent::ServerSideInit()
{
	InitializeBaseAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	// 检查当前组件是否拥有拥有者，并且拥有者是否具有网络权限（权威性） 
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		// 创建游戏效果规格句柄，用于描述要应用的效果及其上下文
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		// 将游戏效果应用到自身
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	// 检查当前组件是否拥有拥有者，并且拥有者是否具有网络权限（权威性） 
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<ECAbilityInputID,TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		// 赋予技能 等级为 1
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, static_cast<int32>(AbilityPair.Key), nullptr));
	}
	
	for (const TPair<ECAbilityInputID,TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, static_cast<int32>(AbilityPair.Key), nullptr));
	}

	// for (const auto& [InputID, AbilityClass] : Abilities)
	// {
	// 	GiveAbility(FGameplayAbilitySpec(AbilityClass, 0, static_cast<int32>(InputID), nullptr));
	// }
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& UCAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 获取当前最大生命值
	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(UCAttributeSet::GetMaxHealthAttribute(), bFound);
    
	// 如果生命值达到最大值，添加生命值已满标签
	if (bFound && ChangeData.NewValue >= MaxHealth)
	{
		if (!HasMatchingGameplayTag(TGameplayTags::Stats_Health_Full))
		{
			// 仅本地会添加标签
			AddLooseGameplayTag(TGameplayTags::Stats_Health_Full);
		}
	}
	else
	{
		// 移除生命值已满标签
		RemoveLooseGameplayTag(TGameplayTags::Stats_Health_Full);
	}
	if (ChangeData.NewValue <= 0.0f)
	{
		if (!HasMatchingGameplayTag(TGameplayTags::Stats_Health_Empty))
		{
			// 本地添加生命值清零标签
			AddLooseGameplayTag(TGameplayTags::Stats_Health_Empty);
			// 角色死亡
			if (DeathEffect)
			{
				AuthApplyGameplayEffect(DeathEffect);
			}
		}
	}else
	{
		RemoveLooseGameplayTag(TGameplayTags::Stats_Health_Empty);
	}
}

void UCAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	// 仅在拥有者存在且为服务器时执行
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 获取当前最大魔法值
	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(UCAttributeSet::GetMaxManaAttribute(), bFound);
    
	// 如果魔法值达到最大值，添加魔法值已满标签
	if (bFound && ChangeData.NewValue >= MaxMana)
	{
		if (!HasMatchingGameplayTag(TGameplayTags::Stats_Mana_Full))
		{
			// 仅本地生效的标签
			AddLooseGameplayTag(TGameplayTags::Stats_Mana_Full);
		}
	}
	else
	{
		// 移除魔法值已满标签
		RemoveLooseGameplayTag(TGameplayTags::Stats_Mana_Full);
	}

	// 处理魔法值为零的情况
	if (ChangeData.NewValue <= 0)
	{
		if (!HasMatchingGameplayTag(TGameplayTags::Stats_Mana_Empty))
		{
			// 添加魔法值清零标签
			AddLooseGameplayTag(TGameplayTags::Stats_Mana_Empty);
		}
	}
	else
	{
		// 移除魔法值清零标签
		RemoveLooseGameplayTag(TGameplayTags::Stats_Mana_Empty);
	}
}