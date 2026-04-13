# 经济系统

[返回首页](Home.md) | [装备系统](Items.md)

## 概述

金币是对局中的唯一货币，用于在商店购买装备。金币通过击杀敌方单位获取。

## 金币来源

### 击杀英雄

由 `GAP_Dead`（死亡被动技能）处理金币和经验分配：

- **击杀者** — 获得基础击杀赏金
- **助攻者** — 参与击杀的队友获得助攻赏金
- **连杀加成** — 连续击杀的玩家被击杀时，赏金更高
- **连败补偿** — 连续死亡的玩家给予的赏金递减，同时该玩家复活后获得补偿金

### 击杀小兵

击杀敌方小兵获得固定金币和经验。

## 装备购买

- 装备从商店购买，详见 [装备系统](Items.md)
- 出售装备返还 **50%** 购买价
- 合成装备时自动扣除已有材料价值

## 经验与等级

经验通过 `CHeroAttributeSet` 管理：

- 击杀英雄/小兵获得经验
- 经验达到 `NextLevelExperience` 时升级
- 升级获得 `UpgradePoint`（技能点）
- 技能点分配给 Q/E/F/R 提升技能等级
- 最高等级由 `MaxLevel` 属性限定

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/GAS/Abilities/GAP_Dead.h/.cpp` | 死亡处理（击杀/助攻赏金分配、连杀连败） |
| `Source/Crunch/Private/GAS/Core/CHeroAttributeSet.h/.cpp` | 英雄属性集（Gold、Experience、Level） |
| `Source/Crunch/Private/Inventory/InventoryComponent.h` | 购买/出售金币扣除逻辑 |
