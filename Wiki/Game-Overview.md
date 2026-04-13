# 游戏概述

[返回首页](Home.md)

## 游戏类型

Crunch 是一款 5v5 MOBA（多人在线竞技场）游戏。两支队伍在对称地图上对抗，最终目标是将**风暴核心**推入敌方基地。

## 游戏流程

### 1. 主菜单

玩家登录后可以创建或加入房间。使用 Epic Online Services (EOS) 管理会话。

### 2. 角色选择（大厅）

- 每个玩家选择一个插槽（决定队伍归属）和一个英雄
- 同一局中每个英雄只能被选择一次
- 全员确认后开始对局

### 3. 对局

- 玩家操控英雄在地图上移动、战斗
- 击杀敌方英雄和小兵获取**金币**和**经验**
- 在商店购买**装备**强化属性
- 升级时获得**技能点**，分配给 Q/E/F/R 四个技能
- 地图中央有**风暴核心**，队伍在核心范围内施加影响力

### 4. 胜利条件

风暴核心根据两队影响力差值移动。当核心到达某队的目标点时，该队获胜。

## 操作

| 按键 | 操作 |
| ---- | ---- |
| WASD | 移动 |
| 鼠标 | 视角控制 |
| 左键 | 普攻 |
| Q / E / F / R | 技能 |
| B | 打开商店 |
| Tab | 属性面板 |
| Enter | 聊天 |
| M | 小地图（如有） |

## 核心机制

- [英雄列表](Heroes.md) — 可选英雄
- [属性系统](Attributes.md) — 属性说明
- [装备系��](Items.md) — 商店与合成
- [伤害计算](Damage-Formula.md) — 伤害公式
- [风暴核心](Storm-Core.md) — 胜利目标
- [经济系统](Economy.md) — 金币获取

## 源文件

| 文件 | 说明 |
| ---- | ---- |
| `Source/Crunch/Private/Framework/CGameMode.h/.cpp` | 对局 GameMode |
| `Source/Crunch/Private/Framework/CGameState.h/.cpp` | 对局 GameState（角色选择、击杀追踪） |
| `Source/Crunch/Private/Framework/MGameInstance.h/.cpp` | GameInstance（登录、会话管理） |
| `Source/Crunch/Private/Framework/LobbyGameMode.h/.cpp` | 大厅 GameMode |
| `Source/Crunch/Private/Framework/StormCore.h/.cpp` | 风暴核心 |
