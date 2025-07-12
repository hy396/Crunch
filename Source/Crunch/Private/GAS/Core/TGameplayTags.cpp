// 幻雨喜欢小猫咪


#include "GAS/Core/TGameplayTags.h"

namespace TGameplayTags
{
	//UE_DEFINE_GAMEPLAY_TAG(Ability_BasicAttack, "Ability.BasicAttack")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack, "Ability.BasicAttack", "基础攻击能力");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change, "Ability.Combo.Change", "连击切换")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo01, "Ability.Combo.Change.Combo01", "连击切换连段1")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo02, "Ability.Combo.Change.Combo02", "连击切换连段2")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo03, "Ability.Combo.Change.Combo03", "连击切换连段3")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo04, "Ability.Combo.Change.Combo04", "连击切换连段4")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_End, "Ability.Combo.Change.End", "连击切换结束")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Damage, "Ability.Combo.Damage", "连击伤害")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Uppercut_Launch, "Ability.Uppercut.Launch", "升龙拳攻击")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Uppercut_Cooldown, "Ability.Uppercut.Cooldown", "升龙拳技能冷却")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Passive_Launch_Activate, "Ability.Passive.Launch.Activate", "击飞被动技能激活")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Dead, "Stats.Dead", "死亡")
}
