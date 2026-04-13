# 风暴核心

[返回首页](Home.md)

## 概述

风暴核心（Storm Core）是地图中央的核心目标，决定对局胜负。两支队伍通过在核心周围施加影响力来争夺控制权，推动核心移向敌方基地。

## 机制

### 影响力

`AStormCore` 有一个球形影响范围（默认半径 1000 单位）。当角色进入范围时：

- 统计两队在范围内的角色数量
- 计算净影响力权重：`TeamWeight = Team1Count - Team2Count`
- 正值 → 核心向队伍2的目标移动
- 负值 → 核心向队伍1的目标移动
- 零 → 核心停止移动

### 移动

核心以 AI 寻路方式移动，最大速度 500 单位/秒。移动速度与影响力权重成正比。

### 胜利

核心到达某队目标点时触发 `OnGoalReachedDelegate`：

1. 核心播放捕获蒙太奇
2. `CGameMode::MatchFinished()` 被调用
3. 所有玩家摄像机切换到核心的 `ViewCam`
4. 对局结束

## 关键属性

| 属性 | 默认值 | 说明 |
| ---- | ---- | ---- |
| InfluenceRadius | 1000 | 影响力检测范围 |
| MaxMoveSpeed | 500 | 最大移动速度 |
| TeamOneGoal | 编辑器设置 | 队伍1的目标点 |
| TeamTwoGoal | 编辑器设置 | 队伍2的目标点 |
| TeamOneCore | 编辑器设置 | 队伍1的核心位置 |
| TeamTwoCore | 编辑器设置 | 队伍2的核心位置 |

## 视觉

- `UDecalComponent` 在地面显示影响范围指示
- 展开蒙太奇 (`ExpandMontage`) — 核心激活时播放
- 捕获蒙太奇 (`CaptureMontage`) — 核心被捕获时播放

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/Framework/StormCore.h/.cpp` | 风暴核心 Actor |

关键可配置属性（在编辑器中调整）: `InfluenceRadius`, `MaxMoveSpeed`, `TeamOneGoal/TeamTwoGoal`, `TeamOneCore/TeamTwoCore`, `ExpandMontage/CaptureMontage`
