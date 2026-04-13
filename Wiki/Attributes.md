# 属性系统

[返回首页](Home.md)

## 概述

Crunch 使用 GAS 的 AttributeSet 管理所有数值属性。分为两个属性集：**基础属性集**（所有可战斗角色）和**英雄属性集**（仅玩家角色）。

## 基础属性 (CAttributeSet)

### 资源

| 属性 | 说明 |
| ---- | ---- |
| Health | 当前生命值 |
| MaxHealth | 最大生命值 |
| Mana | 当前法力值 |
| MaxMana | 最大法力值 |

生命值归零触发死亡。法力值由技能消耗，部分技能可回复法力。

### 攻击

| 属性 | 说明 |
| ---- | ---- |
| AttackPower | 物理攻击力，影响物理伤害技能的最终伤害 |
| MagicPower | 魔法攻击力，影响魔法伤害技能的最终伤害 |

### 防御

| 属性 | 说明 |
| ---- | ---- |
| Armor | 护甲，减少受到的物理伤害 |
| MagicResistance | 魔抗，减少受到的魔法伤害 |

### 穿透

| 属性 | 说明 |
| ---- | ---- |
| ArmorPenetration | 固定护甲穿透（从目标护甲中直接扣除） |
| ArmorPenetrationPercent | 百分比护甲穿透（穿透后再乘以百分比） |
| MagicPenetration | 固定魔抗穿透 |
| MagicPenetrationPercent | 百分比魔抗穿透 |

穿透计算顺序：先固定穿透，后百分比穿透。详见 [伤害计算](Damage-Formula.md)。

### 通用修正

| 属性 | 说明 |
| ---- | ---- |
| MoveSpeed | 移动速度 |
| DamageAmplification | 伤害加深（百分比，增加造成的伤害） |
| DamageReduction | 伤害减免（百分比，减少受到的伤害） |

### Meta 属性

| 属性 | 说明 |
| ---- | ---- |
| AttackDamage | 物理伤害传递值 |
| MagicDamage | 魔法伤害传递值 |
| TrueDamage | 真实伤害传递值 |

Meta 属性是伤害计算的中间变量。`ECC_AttackDamage` 计算最终伤害后写入这些属性，`PostGameplayEffectExecute` 读取后扣除生命值并立即清零。这些属性**不进行网络同步**，仅在服务器使用。

## 英雄属性 (CHeroAttributeSet)

仅玩家角色拥有，用于成长系统：

| 属性 | 说明 |
| ---- | ---- |
| Experience | 当前经验值 |
| PrevLevelExperience | 当前等级起始经验 |
| NextLevelExperience | 升级所需经验 |
| Level | 当前等级 |
| UpgradePoint | 可用技能点 |
| MaxLevel | 最高等级（固定值） |
| MaxLevelExperience | 满级经验（固定值） |
| Gold | 当前金币 |

## 属性来源

属性可以被以下方式修改：

- **角色初始化** — `PDA_CharacterDefinition` 中的 `InitStatEffect` 设置基础值
- **等级提升** — GameplayEffect 按等级增加属性
- **装备** — `UPDA_ShopItem` 的 `EquippedEffects` 持续修改属性
- **技能 Buff/Debuff** — 临时修改属性（减甲、减速等）
- **伤害** — 通过 Meta 属性扣除生命值

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/GAS/Core/CAttributeSet.h/.cpp` | 基础属性集 |
| `Source/Crunch/Private/GAS/Core/CHeroAttributeSet.h/.cpp` | 英雄属性集 |
| `Source/Crunch/Private/GAS/Executions/ECC_AttackDamage.h/.cpp` | 伤害执行计算（读取属性） |
| `docs/GAS_Optimization_Changelog.md` | 属性同步优化记录 |
