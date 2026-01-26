// 幻雨喜欢小猫咪


#include "GAS/Core/CAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "CAbilitySystemStatics.h"
#include "CAttributeSet.h"
#include "CHeroAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "TGameplayTags.h"
#include "Player/CPlayerCharacter.h"
#include "Player/CPlayerController.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated);
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &UCAbilitySystemComponent::ManaUpdated);
	GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &UCAbilitySystemComponent::ExperienceUpdated);
	GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetGoldAttribute()).AddUObject(this, &UCAbilitySystemComponent::HandleGoldChanged);
	
	GenericConfirmInputID = static_cast<int32>(ECAbilityInputID::Confirm);
	GenericCancelInputID = static_cast<int32>(ECAbilityInputID::Cancel);
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!AbilitySystemGenerics || !AbilitySystemGenerics->GetBaseStatDataTable() || !GetOwner())
	{
		return;
	}
	// 获取基础属性数据表和角色对应的配置数据
	const UDataTable* BaseStatDataTable = AbilitySystemGenerics->GetBaseStatDataTable();
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
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxHealthGrowthRateAttribute(), BaseStats->MaxHealthGrowthRate);				// 生命成长
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxManaGrowthRateAttribute(), BaseStats->MaxManaGrowthRate);					// 法力成长
		SetNumericAttributeBase(UCHeroAttributeSet::GetAttackPowerGrowthRateAttribute(), BaseStats->AttackPowerGrowthRate);			// 攻击成长率
		SetNumericAttributeBase(UCHeroAttributeSet::GetMagicPowerGrowthRateAttribute(), BaseStats->MagicPowerGrowthRate);			// 法术成长率
		SetNumericAttributeBase(UCHeroAttributeSet::GetHealthRegenGrowthRateAttribute(), BaseStats->AttackPowerGrowthRate);			// 生命回复成长
		SetNumericAttributeBase(UCHeroAttributeSet::GetManaRegenGrowthRateAttribute(), BaseStats->MagicPowerGrowthRate);			// 法力回复成长
		SetNumericAttributeBase(UCHeroAttributeSet::GetArmorGrowthRateAttribute(), BaseStats->ArmorGrowthRate);						// 护甲成长
		SetNumericAttributeBase(UCHeroAttributeSet::GetMagicResistanceGrowthRateAttribute(), BaseStats->MagicResistanceGrowthRate);	// 法术抗性成长
	}

	// 处理经验系统配置
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (ExperienceCurve)
	{
		int MaxLevel = ExperienceCurve->GetNumKeys(); // 经验曲线中的最大等级
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel); // 设置角色最大等级限制

		float MaxExp = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle()); // 最高等级所需经验
		SetNumericAttributeBase(UCHeroAttributeSet::GetMaxLevelExperienceAttribute(), MaxExp); // 设置最高等级经验阈值

		// 输出调试信息
		UE_LOG(LogTemp, Warning, TEXT("最大等级为: %d, 最大经验值为: %f"), MaxLevel, MaxExp);
	}
	// 触发经验属性更新（用于初始化等级相关状态）
	ExperienceUpdated(FOnAttributeChangeData());
}

void UCAbilitySystemComponent::ServerSideInit()
{
	InitializeBaseAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
	
}

void UCAbilitySystemComponent::ShowComboText(float Amount, ECurrencyType CurrencyType, const FVector& HitLocation)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 获取当前Owner的控制器，如果该控制器实现了ICombatTextInterface接口，则调用其显示伤害数字的方法
	if (ACPlayerCharacter* PlayerCharacter = Cast<ACPlayerCharacter>(GetOwner()))
	{
		if (ACPlayerController* PlayerController = Cast<ACPlayerController>(PlayerCharacter->GetController()))
		{
			PlayerController->Client_ShowCombatText(Amount, PlayerCharacter, CurrencyType);
		}
	}
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	// 检查当前组件是否拥有拥有者，并且拥有者是否具有网络权限（权威性） 
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (!AbilitySystemGenerics)
		return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialEffects())
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
	
	if (!AbilitySystemGenerics)
		return;

	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(PassiveAbility, 1, -1, nullptr));
	}
	// for (const auto& [InputID, AbilityClass] : Abilities)
	// {
	// 	GiveAbility(FGameplayAbilitySpec(AbilityClass, 0, static_cast<int32>(InputID), nullptr));
	// }
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	if (!AbilitySystemGenerics || !AbilitySystemGenerics->GetFullStatEffect())
		return;
	AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatEffect());
}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& UCAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
}

bool UCAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound;
	float CurrentLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	float MaxLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetMaxLevelAttribute(), bFound);
	return CurrentLevel >= MaxLevel;
}

void UCAbilitySystemComponent::Server_UpgradeAbilityWithID_Implementation(ECAbilityInputID InputID)
{
	// 获取可用升级点数
	bool bFound = false;
	float UpgradePoint = GetGameplayAttributeValue(UCHeroAttributeSet::GetUpgradePointAttribute(), bFound);
	// 检查可用升级点数是否大于0
	if (!bFound || UpgradePoint <= 0) return;

	// 获取玩家等级
	float CurrentLevel = GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;
	
	// 获取对应输入ID的技能
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID(static_cast<int32>(InputID));
	// 检查是否有该技能以及等级是否满级
	if (!AbilitySpec || UCAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec,CurrentLevel))
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("技能升级成功%d"),InputID)
	// 消耗一个技能点升级技能
	SetNumericAttributeBase(UCHeroAttributeSet::GetUpgradePointAttribute(), UpgradePoint - 1);
	AbilitySpec->Level += 1;
	// 标记 FGameplayAbilitySpec 状态已改变，通知 GAS 需要将其复制到客户端
	// （直接修改AbilitySpec成员后必须调用此函数）
	MarkAbilitySpecDirty(*AbilitySpec);

	// 通知客户端更新技能等级
	Client_AbilitySpecLevelUpdated(AbilitySpec->Handle, AbilitySpec->Level);
}

bool UCAbilitySystemComponent::Server_UpgradeAbilityWithID_Validate(ECAbilityInputID InputID)
{
	return true;
}

void UCAbilitySystemComponent::Client_AbilitySpecLevelUpdated_Implementation(FGameplayAbilitySpecHandle Handle,
	int NewLevel)
{
	// 通过句柄查找本地技能实例
	if (FGameplayAbilitySpec* const Spec = FindAbilitySpecFromHandle(Handle))
	{
		// 更新客户端技能等级
		Spec->Level = NewLevel;
		
		// 广播变更通知，刷新等客户端响应
		AbilitySpecDirtiedCallbacks.Broadcast(*Spec);
	}
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
			if(AbilitySystemGenerics && AbilitySystemGenerics->GetDeathEffect())
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());

			// TODO:这里是由GE直接扣血的时候触发这种的死亡，我使用的是ECC触发的方式是在属性这边发送事件
			// // 创建需要传给死亡被动技能的事件数据
			// FGameplayEventData DeadAbilityEventData;
			// if (ChangeData.GEModData)
			// {
			// 	DeadAbilityEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
			// }else
			// {
			// 	UE_LOG(LogTemp, Error, TEXT("ChangeData.GEModData is null"))
			// }
			// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), 
			// 	TGameplayTags::Stats_Dead, 
			// 	DeadAbilityEventData);
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

void UCAbilitySystemComponent::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	// 仅在拥有者存在且为服务器时执行
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 满级返回
	if (IsAtMaxLevel()) return;

	// 检查能力系统通用配置是否有效
	if (!AbilitySystemGenerics)
		return;

	// 获取当前经验值
	float CurrentExp = ChangeData.NewValue;
	
	// 从配置中获取经验曲线数据（等级->所需经验的映射）
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (!ExperienceCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("无法找到经验数据!"));
		return;
	}

	float PrevLevelExp = 0.f;	// 当前等级的最低经验值
	float NextLevelExp = 0.f;	// 下一级所需最低经验值
	float NewLevel = 1.f;		// 新的等级

	for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter)
	{
		// 获取当前等级（NewLevel）对应的升级经验阈值
		float ExperienceToReachLevel = ExperienceCurve->GetKeyValue(*Iter);

		if (CurrentExp < ExperienceToReachLevel)
		{
			// 找到第一个大于当前经验的等级阈值
			NextLevelExp = ExperienceToReachLevel;
			break;
		}
		// 记录当前等级的最低经验值
		PrevLevelExp = ExperienceToReachLevel;
		NewLevel = Iter.GetIndex() + 1;	// 等级加一
	}
	// 获取当前等级以及可用的升级点数
	float CurrentLevel = GetNumericAttributeBase(UCHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoint = GetNumericAttribute(UCHeroAttributeSet::GetUpgradePointAttribute());

	// UE_LOG(LogTemp, Warning, TEXT("Level:%f"),CurrentLevel);
	// 计算等级提升数
	float LevelUpgraded = NewLevel - CurrentLevel;
	// 累加升级点数(当前点数+升级的级数)
	float NewUpgradePoint = CurrentUpgradePoint + LevelUpgraded;

	// 更新角色的属性值
	SetNumericAttributeBase(UCHeroAttributeSet::GetLevelAttribute(), NewLevel);					  // 设置新等级
	SetNumericAttributeBase(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelExp); // 设置当前等级经验基准
	SetNumericAttributeBase(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelExp); // 设置下等级经验基准
	SetNumericAttributeBase(UCHeroAttributeSet::GetUpgradePointAttribute(), NewUpgradePoint);     // 更新可分配升级点数

	// 显示经验获取数字
	ShowComboText(ChangeData.NewValue - ChangeData.OldValue, ECurrencyType::Experience, GetOwner()->GetActorLocation());
	
}

void UCAbilitySystemComponent::HandleGoldChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	// 是否为英雄单位
	if (HasMatchingGameplayTag(TGameplayTags::Role_Hero))
	{
		// 只统计增加的金币，不统计减少的金币
		if (OnAttributeChangeData.NewValue > OnAttributeChangeData.OldValue)
		{
			// AddGoldEarnedMatchStatNumber(OnAttributeChangeData.NewValue - OnAttributeChangeData.OldValue);

			// 显示金币获取数字
			if (OnAttributeChangeData.GEModData)
			{
				FVector HitLocation = GetOwner()->GetActorLocation();
				if (OnAttributeChangeData.GEModData->EffectSpec.GetContext().GetHitResult())
				{
					HitLocation = OnAttributeChangeData.GEModData->EffectSpec.GetContext().GetHitResult()->Location;
				}
				
				ShowComboText(OnAttributeChangeData.NewValue - OnAttributeChangeData.OldValue, ECurrencyType::Gold, HitLocation);
			}
		}
	}
}