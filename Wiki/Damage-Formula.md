# 伤害计算

[返回首页](Home.md) | [属性系统](Attributes.md)

## 概述

所有伤害通过 `ECC_AttackDamage`（Gameplay Effect Execution Calculation）计算。支持三种伤害类型：物理、魔法、真实。

## 伤害类型

| 类型 | 受什么影响 | 穿透属性 |
| ---- | ---- | ---- |
| 物理伤害 | 护甲 (Armor) | ArmorPenetration, ArmorPenetrationPercent |
| 魔法伤害 | 魔抗 (MagicResistance) | MagicPenetration, MagicPenetrationPercent |
| 真实伤害 | 无防御减免 | 无 |

三种伤害类型都受到 **DamageReduction**（减免）和 **DamageAmplification**（加深）影响。

## 物理/魔法伤害公式

以物理伤害为例（魔法伤害用 MagicResistance 和魔法穿透替换即可）：

### 第一步：计算有效护甲

```text
EffectiveArmor = max(0, Armor - ArmorPenetration)       // 固定穿透
EffectiveArmor = EffectiveArmor * (1 - ArmorPenPercent)  // 百分比穿透
```

固定穿透先生效，然后百分比穿透作用于剩余值。

### 第二步：计算护甲减免率

```text
ArmorReduction = EffectiveArmor / (EffectiveArmor + 100)
```

这个公式使护甲收益递减：
- 0 护甲 → 0% 减免
- 100 护甲 → 50% 减免
- 200 护甲 → 66.7% 减免
- 300 护甲 → 75% 减免

### 第三步：应用减免和加深

```text
FinalDamage = BaseDamage * (1 - ArmorReduction) * (1 - DamageReduction) * (1 + DamageAmplification)
```

## 真实伤害公式

真实伤害跳过穿透和护甲计算：

```text
FinalDamage = BaseDamage * (1 - DamageReduction) * (1 + DamageAmplification)
```

## 伤害来源

技能的伤害通过 `FGenericDamageEffectDef` 定义：

- `DamageEffect` — 伤害 GE 蓝图类
- `DamageTypeDefinitions` — 伤害类型映射，每种类型包含：
  - `BaseDamage` — 基础伤害（按技能等级缩放的 `FScalableFloat`）
  - `AttributeModifiers` — 属性加成系数（如 0.8 * AttackPower）

最终的 BaseDamage = 基础值 + 各属性加成之和。

## 伤害流程

```text
技能调用 ApplyDamageToActor() 或 ApplyDamageToTargetDataHandle()
  → MakeDamageEffectContext()  创建上下文
  → ApplyDamageSpecToTarget()  循环每种伤害类型
    → MakeOutgoingSpec()  创建 GE Spec
    → SetByCaller 设置 BaseDamage
    → ApplyGameplayEffectSpecToTarget()
      → ECC_AttackDamage::Execute_Implementation()  执行计算
        → 读取攻击者/防御者属性
        → ApplyPenetration()  计算穿透后的有效防御
        → ApplyDamageReduction()  计算最终伤害
        → 写入 Meta 属性 (AttackDamage/MagicDamage/TrueDamage)
      → PostGameplayEffectExecute()
        → 读取 Meta 属性
        → 扣除 Health
        → 清零 Meta 属性
        → 触发 GameplayCue（伤害数字、镜头震动）
```

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/GAS/Executions/ECC_AttackDamage.h/.cpp` | 伤害执行计算 |
| `Source/Crunch/Private/GAS/Core/CGameplayAbility.h/.cpp` | 技能基类（伤害管线入口） |
| `Source/Crunch/Private/GAS/Core/CAttributeSet.h/.cpp` | 属性集（PostGameplayEffectExecute 扣血） |
