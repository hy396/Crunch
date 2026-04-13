# 英雄列表

[返回首页](Home.md)

## 概述

每个英雄有独特的技能组合，通过 `UPDA_CharacterDefinition` 数据资产定义外观、技能表和初始属性。对局中每个英雄只能被一名玩家选择。

## 已实现英雄

### 法师

远程魔法输出，技能偏向 AOE 和控制。

| 按键 | 技能 | 说明 |
| ---- | ---- | ---- |
| Q | [奥术弹](Hero-Mage.md#奥术弹) | 穿透能量球，距离越远伤害越高，命中回蓝 |
| E | [相位转移](Hero-Mage.md#相位转移) | 无敌后撤，原地放置定身陷阱 |
| F | [陨石打击](Hero-Mage.md#陨石��击) | 地面选点，延迟落地 + 燃烧地面 DOT |
| R | [奥术风暴](Hero-Mage.md#奥术风暴) | 大招：持续 AOE 减速，最终爆发沉默 |

详见 [法师详细页面](Hero-Mage.md)。

### 大斧战士

近战物理输出，技能偏向 AOE 伤害和硬控。

| 按键 | 技能 | 说明 |
| ---- | ---- | ---- |
| Q | [战吼](Hero-Warrior.md#战吼) | AOE 伤害 + 减甲减速 |
| E | [断头台](Hero-Warrior.md#断头台) | 锥形劈砍，低血量斩杀加成 + 流血 |
| F | [掷斧回旋](Hero-Warrior.md#掷斧回旋) | 回旋飞斧，去程回程双段伤害 |
| R | [地裂](Hero-Warrior.md#地裂) | 大招：三段砸地，眩晕 + 击飞 |

详见 [战士详细页面](Hero-Warrior.md)。

## 通用技能

以下技能可能出现在多个英雄的技能表中：

| 技能 | 类型 | 说明 |
| ---- | ---- | ---- |
| 连击 (GA_Combo) | 普攻 | 多段近战连击 |
| 射击 (GA_Shoot) | 远程 | 远程射击 |
| 连锁攻击 (GA_ChainAttack) | 连锁 | 对多个目标连续攻击 |
| 冲刺 (GA_Dash) | 位移 | 短距离冲刺 |
| 闪现 (GA_Blink) | 位移 | 瞬间位移 |
| 冰冻 (GA_Freeze) | 控制 | 冻结目标 |
| 龙��风 (GA_Tornado) | 控制 | 龙卷风效果 |
| 黑洞 (GA_BlackHole) | 控制 | 吸引范围内敌人 + 持续伤害 |
| 地面冲击 (GA_GroundBlast) | 伤害 | 地面范围伤害 |
| 激光 (GA_Laser) | 伤害 | 激光射线 |
| 剑气 (GA_SwordAura) | 伤害 | 远程剑气 |
| 上挑 (UpperCut) | 伤害 | 上挑攻击 |

## 被动技能

| 技能 | 说明 |
| ---- | ---- |
| 死亡处理 (GAP_Dead) | 击杀/助攻金币经验分配，连杀连败赏金 |
| 击飞处理 (GAP_Launched) | 击飞状态下的物理表现 |

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/Character/PDA_CharacterDefinition.h/.cpp` | 角色数据资产（技能表、外观、初始属性） |
| `Source/Crunch/Private/Character/CCharacter.h/.cpp` | 角色基类 |
| `Source/Crunch/Private/Character/CPlayerCharacter.h/.cpp` | 玩家角色 |
| `Source/Crunch/Private/GAS/Core/CAbilitySystemComponent.h/.cpp` | ASC（技能初始化、升级） |
