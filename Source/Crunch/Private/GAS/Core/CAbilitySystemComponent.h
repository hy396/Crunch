// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CGameplayAbilityTypes.h"
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
private:
	// 初始效果
	void ApplyInitialEffects();
	// 技能初始化
	void GiveInitialAbilities();
	// 添加GE
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);

	// 满血、满蓝效果
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;
	// 死亡效果
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	// 基础技能
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	// 技能
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	// 基础属性数据表
	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	TObjectPtr<UDataTable> BaseStatDataTable;
};
