// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CGameplayAbilityTypes.h"
#include "GAS/Data/PDA_AbilitySystemGenerics.h"
#include "CAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UCAbilitySystemComponent();

	// 初始化基础属性
	void InitializeBaseAttributes();
	// 服务器初始化
	void ServerSideInit();
	// 回满血、满蓝效果
	void ApplyFullStatEffect();

	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
	// 是否达到最大等级
	bool IsAtMaxLevel() const;
	
	/**
	 * 服务器端处理能力升级请求
	 * 通过指定的ECAbilityInputID参数升级对应能力
	 * 包含可靠的网络验证机制
	 * @param InputID - 要升级的能力输入ID
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithID(ECAbilityInputID InputID);
	
	/**
	 * 客户端能力等级更新同步
	 * 当能力等级发生变化时触发网络同步
	 * 通过GameplayAbilitySpecHandle定位具体能力实例
	 * @param Handle - 能力实例句柄
	 * @param NewLevel - 新的能力等级数值
	 */
	UFUNCTION(Client, Reliable)
	void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int NewLevel);
	
private:
	// 初始效果
	void ApplyInitialEffects();
	// 技能初始化
	void GiveInitialAbilities();
	// 添加GE
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData);

	// // 满血、满蓝效果
	// UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	// TSubclassOf<UGameplayEffect> FullStatEffect;
	// // 死亡效果
	// UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	// TSubclassOf<UGameplayEffect> DeathEffect;
	//
	// UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	// TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	// 基础技能
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	// 技能
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	// // 被动技能
	// UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	// TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	//
	// // 基础属性数据表
	// UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	// TObjectPtr<UDataTable> BaseStatDataTable;

	// 能力系统通用配置
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TObjectPtr<UPDA_AbilitySystemGenerics> AbilitySystemGenerics;
};
