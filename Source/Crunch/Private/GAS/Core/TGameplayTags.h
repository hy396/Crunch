// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameplayTagContainer.h"

/**
 * 
 */
namespace TGameplayTags
{
	// 基础攻击技能
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_BasicAttack)
	
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo01)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo02)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo03)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_Combo04)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Change_End)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Combo_Damage)

	// 升龙拳
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Uppercut_Launch)
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Uppercut_Cooldown)
	// 击飞被动
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Passive_Launch_Activate)
	// 死亡
	CRUNCH_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stats_Dead)
}
