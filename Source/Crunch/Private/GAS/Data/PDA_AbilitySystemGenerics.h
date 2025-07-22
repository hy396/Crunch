// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "PDA_AbilitySystemGenerics.generated.h"

/**
 * 通用能力系统数据资产类
 * 存储游戏所有角色能力系统的公共配置数据
 * 包含基础属性、游戏效果、被动技能等配置信息
 */
UCLASS()
class CRUNCH_API UPDA_AbilitySystemGenerics : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 获取完整属性效果类
	// 用于初始化角色基础属性值
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatEffect() const { return FullStatEffect; }

	// 获取死亡效果类
	// 角色死亡时应用的全局游戏效果
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const { return DeathEffect; }

	// 获取初始效果数组
	// 角色初始化时自动应用的游戏效果集合
	FORCEINLINE const TArray<TSubclassOf<UGameplayEffect>>& GetInitialEffects() const { return InitialEffects; }

	// 获取被动技能数组
	// 角色默认解锁的被动技能列表
	FORCEINLINE const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const { return PassiveAbilities; }

	// 获取基础属性数据表
	// 存储角色基础属性（力量、敏捷等）的DataTable资源
	FORCEINLINE const UDataTable* GetBaseStatDataTable() const { return BaseStatDataTable; }

	// 获取经验曲线数据
	// 用于计算角色升级所需经验值的曲线
	const FRealCurve* GetExperienceCurve() const;

private:
	// 全局属性效果
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;

	// 死亡惩罚效果
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;

	// 初始效果列表
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	// 默认被动技能列表
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;

	// 基础属性数据表资源
	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	TObjectPtr<UDataTable> BaseStatDataTable;

	// 经验等级曲线名称
	// 指定经验曲线表中使用的行名称
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName ExperienceRowName = "ExperienceNeededToReachLevel";

	// 经验曲线资源
	// 存储等级-经验值对应关系的曲线表
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TObjectPtr<UCurveTable> ExperienceCurveTable;
};