// 幻雨喜欢小猫咪


#include "GAS/Core/TGameplayTags.h"

namespace TGameplayTags
{
	//UE_DEFINE_GAMEPLAY_TAG(Ability_BasicAttack, "Ability.BasicAttack")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack, "Ability.BasicAttack", "基础攻击能力");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack_Pressed, "Ability.BasicAttack.Pressed", "按下普通攻击")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack_Released, "Ability.BasicAttack.Released", "释放普通攻击键")
	
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change, "Ability.Combo.Change", "连击切换")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo01, "Ability.Combo.Change.Combo01", "连击切换连段1")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo02, "Ability.Combo.Change.Combo02", "连击切换连段2")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo03, "Ability.Combo.Change.Combo03", "连击切换连段3")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo04, "Ability.Combo.Change.Combo04", "连击切换连段4")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_CycloneSlashHold, "Ability.Combo.Change.CycloneSlashHold", "空中持续释放旋风斩")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_End, "Ability.Combo.Change.End", "连击切换结束")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Damage, "Ability.Combo.Damage", "连击伤害")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Uppercut_Launch, "Ability.Uppercut.Launch", "升龙拳攻击")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Uppercut_Cooldown, "Ability.Uppercut.Cooldown", "升龙拳技能冷却")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_GroundBlast_Cooldown, "Ability.GroundBlast.Cooldown", "大地爆炸技能冷却")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Passive_Launch_Activate, "Ability.Passive.Launch.Activate", "击飞被动技能激活")

	// 射击
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Shoot, "Ability.Shoot", "射击技能")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Shoot_Hand_l, "Ability.Shoot.Hand_l", "左手开火")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Shoot_Hand_r, "Ability.Shoot.Hand_r", "右手开火")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Laser_Shoot, "Ability.Laser.Shoot", "激光技能")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Laser_Cooldown, "Ability.Laser.Cooldown", "激光技能冷却")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BlackHole_Cooldown, "Ability.BlackHole.Cooldown", "黑洞技能冷却")

	// 冲刺
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dash, "Ability.Dash", "冲刺技能")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dash_Start, "Ability.Dash.Start", "冲刺技能开始")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dash_Cooldown, "Ability.Dash.Cooldown", "冲刺技能冷却")

	// 龙卷风冷却
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Tornado_Cooldown, "Ability.Tornado.Cooldown", "龙卷风技能冷却")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Freeze_Cooldown, "Ability.Freeze.Cooldown", "冻结技能冷却")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Blink_Teleport, "Ability.Blink.Teleport", "闪烁技能")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Blink_Cooldown, "Ability.Blink.Cooldown", "闪烁技能冷却")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ChainAttack, "Ability.ChainAttack", "连锁攻击技能")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ChainAttack_Cooldown, "Ability.ChainAttack.Cooldown", "连锁攻击技能冷却")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_SwordAura, "Ability.SwordAura", "剑气技能")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_SwordAura_Cooldown, "Ability.SwordAura.Cooldown", "剑气技能冷却")

	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Generic_Damage, "Ability.Generic.Damage", "技能伤害")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Generic_Target, "Ability.Generic.Target", "技能目标")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Dead, "Stats.Dead", "死亡")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Stun, "Stats.Stun", "眩晕")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Aim, "Stats.Aim", "瞄准")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Crosshair, "Stats.Crosshair", "准星")
	//目标更新
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Target_Updated, "Target.Updated", "目标更新")

	// 聚焦
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Focus, "Stats.Focus", "聚焦")
	// 状态效果
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Debuff, "Stats.Debuff", "负面状态效果")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Buff, "Stats.Buff", "正面状态效果")
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Health_Full, "Stats.Health.Full", "生命值满")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Mana_Full, "Stats.Mana.Full", "法术值满")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Health_Empty, "Stats.Health.Empty", "生命值空")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Mana_Empty, "Stats.Mana.Empty", "法术值空")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Role_Hero, "Role.Hero", "英雄角色")

	// 属性
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AttributeSet_Experience, "AttributeSet.Experience", "经验值")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AttributeSet_Gold, "AttributeSet.Gold", "金币")

	// 伤害类型
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageType_AttackDamage, "DamageType.AttackDamage", "物理伤害")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageType_MagicDamage, "DamageType.MagicDamage", "魔法伤害")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageType_TrueDamage, "DamageType.TrueDamage", "真实伤害")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxHealth, "Attribute.MaxHealth", "最大生命值")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "Attribute.Health", "生命值")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxMana, "Attribute.MaxMana", "最大法术值")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Mana, "Attribute.Mana", "法术值")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_AttackPower, "Attribute.AttackPower", "攻击力")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MagicPower, "Attribute.MagicPower", "魔法强度")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Armor, "Attribute.Armor", "护甲")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MagicResistance, "Attribute.MagicResistance", "魔法抗性")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MoveSpeed, "Attribute.MoveSpeed", "移动速度")
}