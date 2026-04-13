# Walkthrough: 添加新装备

[返回首页](Home.md)

## 概述

Crunch 的装备系统是数据驱动的。每件装备是一个 `UPDA_ShopItem` 数据资产（`UPrimaryDataAsset`），在编辑器中配置属性，不需要写 C++ 代码（除非装备有主动技能）。

## Step 1: 创建数据资产

1. 在编辑器 Content Browser 中右键 -> Miscellaneous -> Data Asset
2. 选择 `PDA_ShopItem` 作为类
3. 命名（如 `DA_Item_LongSword`）

## Step 2: 配置基础属性

在 Details 面板中设置：

| 属性 | 说明 | 示例 |
| ---- | ---- | ---- |
| `ItemName` | 显示名称 | "长剑" |
| `ItemDescription` | 物品描述 | "增加攻击力" |
| `Icon` | 图标（软引用 Texture2D） | T_Icon_LongSword |
| `Price` | 购买价格 | 350 |
| `bIsConsumable` | 是否消耗品 | false |
| `bIsStackable` | 是否可堆叠 | false |
| `MaxStackCount` | 最大堆叠（堆叠时才需要） | 1 |

出售价格自动为购买价的 50%，由 `GetSellPrice()` 计算。

## Step 3: 创建装备效果 (GameplayEffect)

装备效果是持续型 GE，在物品进入背包时应用，移除时撤销。

1. 新建 GE 蓝图（父类 `GameplayEffect`）
2. Duration Policy: `Infinite`（装备后一直生效）
3. 添加 Modifier：
   - Attribute: `CAttributeSet.AttackPower`
   - ModifierOp: `Additive`
   - ModifierMagnitude: Fixed Value（如 +15）
4. 在 `PDA_ShopItem` 的 `EquippedEffects` 数组中添加这个 GE

可以添加多个 GE 实现复合效果（如同时加攻击力和攻速）。

## Step 4: 创建主动技能（如需要）

如果装备有主动技能（如使用后恢复生命值）：

1. 创建 C++ 类继承 `UGameplayAbility`（放在 `GAS/Abilities/`）
2. 创建蓝图，配置技能逻辑
3. 在 `PDA_ShopItem` 的 `GrantedAbility` 中引用

物品进入背包时自动赋予技能，玩家点击物品或按对应快捷键激活。冷却通过技能的 `CooldownGameplayEffectClass` 管理。

**参考**: `UInventoryItem::TryActivateGrantedAbility()` 和 `GetAbilityCooldownTimeRemaining()`

## Step 5: 创建消耗效果（如需要）

消耗品（药水等）使用后触发一次性效果：

1. 创建 GE 蓝图，Duration Policy: `Instant` 或 `HasDuration`
2. 在 `PDA_ShopItem` 的 `ConsumeEffects` 数组中添加
3. 设置 `bIsConsumable = true`

消耗品使用后堆叠数 -1，堆叠归零时从背包移除。

## Step 6: 设置合成关系

如果这件装备可以由其他装备合成：

1. 在 `PDA_ShopItem` 的 `IngredientItems` 数组中添加材料引用（`TSoftObjectPtr<UPDA_ShopItem>`）
2. 购买时系统自动检测背包中已有的材料，扣除材料价值后按差价购买

合成关系是单向的：只在成品的 `IngredientItems` 中声明材料，材料不需要知道自己能合成什么。

合成树在商店 UI 中通过 `UItemTreeWidget` 自动可视化。

## Step 7: 验证

1. 编辑器中打开商店 Widget，确认新物品出现在商店列表
2. PIE 测试购买、装备效果是否生效
3. 测试出售（应返还 50% 金币）
4. 测试合成（如有）：背包中有材料时购买成品，确认材料被消耗、差价正确
5. 测试主动技能（如有）：点击物品激活，冷却显示

## 关键源文件

| 文件 | 说明 |
| ---- | ---- |
| `Inventory/PDA_ShopItem.h` | 物品数据资产定义 |
| `Inventory/InventoryComponent.h` | 背包逻辑（购买/出售/合成/使用） |
| `Inventory/InventoryItem.h` | 物品实例（堆叠、技能、GAS 集成） |
| `UI/Shop/ShopWidget.h` | 商店 UI |
| `UI/Shop/ItemTreeWidget.h` | 合成树可视化 |
