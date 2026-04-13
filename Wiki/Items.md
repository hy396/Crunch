# 装备系统

[返回首页](Home.md)

## 概述

对局中通过商店消费金币购买装备。装备提供属性加成，部分装备有主动技能。装备可以合成升级。

## 商店

按 **B** 打开商店界面。商店通过 `UPDA_ShopItem` 数据资产驱动，每件物品包含：

- 名称、描述、图标
- 价格
- 装备效果（持续型 GameplayEffect）
- 消耗效果（一次性 GameplayEffect）
- 主动技能（GameplayAbility）
- 是否可堆叠、最大堆叠数
- 合成材料列表

## 合成

装备之间有合成关系，在 `UPDA_ShopItem::IngredientItems` 中定义。

**购买时自动检测合成**：当你购买一件高级装备时，系统自动查找背包中已有的材料，扣除材料后按差价购买。

```text
例：大剑 (1000金) = 长剑 (500金) + 长剑 (500金)

情况1：背包里没有长剑 → 花费 1000 金购买大剑
情况2：背包里有1把长剑 → 花费 500 金，消耗1把长剑，获得大剑
情况3：背包里有2把长剑 → 花费 0 金，消耗2把长剑，获得大剑
```

合成树在商店界面中通过 `UItemTreeWidget` + `USplineWidget` 可视化展示。

## 背包

玩家背包有 **6 个格子**，由 `UInventoryComponent` 管理。

### 操作

| 操作 | 说明 |
| ---- | ---- |
| 购买 | 点击商店物品 → `TryPurchase()` |
| 出售 | 右键物品 → 返还 50% 购买价 |
| 使用 | 点击/快捷键 → 触发主动技能或消耗效果 |
| 拖拽 | 拖拽调整格子位置 |

### 网络流程

所有物品操作都是 Server RPC + 服务器验证：

```text
客户端 TryPurchase()
  → Server_Purchase()  [Server RPC, WithValidation]
  → 服务器验证金币、背包空间
  → 检测可合成材料 (FindIngredientForItem)
  → 扣金币、移除材料、创建物品
  → Client_ItemAdded()  [Client RPC]
  → 客户端更新 UI
```

## 物品效果类型

| 类型 | 说明 | 时机 |
| ---- | ---- | ---- |
| EquippedEffects | 装备效果，持续生效 | 物品进入背包时应用，移除时撤销 |
| ConsumeEffects | 消耗效果，一次性 | 使用消耗品时触发 |
| GrantedAbility | 主动技能 | 进入背包时赋予，使用时激活，有冷却 |

## 数据驱动说明

所有装备都是 `UPDA_ShopItem` 数据资产，在编辑器中配置（不是 C++ 硬编码）。添加新装备不需要写 C++ 代码，除非装备有主动技能。详见 [Walkthrough: 添加新装备](Walkthrough-New-Item.md)。

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/Inventory/PDA_ShopItem.h` | 物品数据资产定义 |
| `Source/Crunch/Private/Inventory/InventoryComponent.h` | 背包组件（购买/出售/合成/使用） |
| `Source/Crunch/Private/Inventory/InventoryItem.h` | 物品实例（堆叠、GAS 集成） |
| `Source/Crunch/Private/UI/Shop/ShopWidget.h` | 商店 UI |
| `Source/Crunch/Private/UI/Shop/ItemTreeWidget.h` | 合成树可视化 |
