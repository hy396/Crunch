# 小兵系统

[返回首页](Home.md)

## 概述

小兵（Minion）是 AI 控制的单位，从兵营周期性刷新，沿固定路线向敌方推进。击杀小兵可获得金币和经验。

## 兵营 (MinionBarrack)

`AMinionBarrack` 负责生成和管理小兵：

- **批量生成** — 按配置的间隔和批次大小周期性生成小兵
- **对象池** — 复用已死亡的小兵实例，减少运行时开销
- **队伍分配** — 生成的小兵自动继承兵营的队伍 ID
- **目标分配** — 为每批小兵设置推进目标点

### 配置

| 属性 | 说明 |
| ---- | ---- |
| SpawnInterval | 每批生成间隔 |
| SpawnGroupSize | 每批小兵数量 |
| MinionClass | 使用的小兵蓝图类 |

## 小兵 (Minion)

`AMinion` 继承 `ACCharacter`，拥有完整的战斗属性和技能系统。

### 特性

- **队伍皮肤** — 根据队伍 ID 自动切换外观
- **目标系统** — 有推进目标（GoalActor）和攻击目标
- **激活状态** — 可设置为非激活状态（刚生成时等待出发）

## AI 控制器 (CAIController)

所有 AI 单位共用 `ACAIController`：

- **行为树** — 驱动 AI 决策（巡逻、追击、攻击）
- **AI 感知** — 视觉感知组件，检测范围内的敌方单位
- **队伍态度** — 通过 `IGenericTeamAgentInterface` 判断敌我
- **状态监控** — 监听死亡/眩晕标签，暂停行为树
- **避障** — 集成 Detour 群体避障系统

### 技能输入

`UBTTask_SendInputToAbilitySystem` 行为树任务节点将 AI 决策转化为 GAS 技能激活：

```text
行为树判断应该攻击
  → BTTask_SendInputToAbilitySystem (InputID = Ability_LMB)
  → ASC->TryActivateAbility()
  → 执行普攻技能
```

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/AI/Minion.h/.cpp` | 小兵 Actor |
| `Source/Crunch/Private/AI/MinionBarrack.h/.cpp` | 兵营（生成器） |
| `Source/Crunch/Private/AI/CAIController.h/.cpp` | AI 控制器 |
| `Source/Crunch/Private/AI/BTTask_SendInputToAbilitySystem.h/.cpp` | 行为树技能输入节点 |

兵营配置属性（编辑器中调整）: `SpawnInterval`, `SpawnGroupSize`, `MinionClass`
