# 系统架构

## 游戏概述

Crunch 是一款 5v5 MOBA 游戏。两支队伍选择英雄，击杀敌方英雄和小兵获取金币经验，购买装备强化自身，最终将风暴核心（Storm Core）推入对方基地获胜。

## 模块依赖

```
Crunch.Build.cs 声明的模块依赖:

Core, CoreUObject, Engine, InputCore, EnhancedInput
├── GameplayAbilities, GameplayTags, GameplayTasks    → GAS 技能系统
├── AIModule, NavigationSystem                         → AI 小兵
├── OnlineSubsystem, OnlineSubsystemEOS, HTTP, Json   → 网络/会话
├── UMG, Slate, SlateCore, CommonUI, CommonInput       → UI
├── MotionWarping                                      → 动画位移
├── Niagara                                            → 特效(伤害数字)
├── AsyncLoadingScreen                                 → 加载界面
└── Networking                                         → 网络底层
```

## 网络拓扑

```
                    ┌───────────────────┐
                    │   Coordinator     │
                    │   (Python)        │
                    │   coordinator.py  │
                    └────────┬──────────┘
                             │ HTTP (创建/查询会话)
              ┌──────────────┼──────────────┐
              ▼              ▼              ▼
     ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
     │  DS (UE5)    │ │  DS (UE5)    │ │  DS (UE5)    │
     │  CGameMode   │ │  CGameMode   │ │  CGameMode   │
     │  CGameState  │ │  CGameState  │ │  CGameState  │
     └──────┬───────┘ └──────┬───────┘ └──────┬───────┘
            │ Replication     │               │
       ┌────┴────┐      ┌───┴───┐       ┌───┴───┐
       │ Clients │      │Clients│       │Clients│
       └─────────┘      └───────┘       └───────┘
```

### 会话生命周期

1. 客户端调用 `UMGameInstance::ClientAccountPortalLogin()` 通过 EOS 登录
2. 创建房间 — `RequestCreateAndJoinSession()` 向 Coordinator 发 HTTP 请求，Coordinator 分配端口后启动 DS
3. 加入房间 — `StartGlobalSessionSearch()` 轮询 EOS 会话列表，`JoinSessionWithId()` 加入
4. 大厅阶段 — `LobbyGameMode` 管理角色选择，`CGameState::PlayerSelectionArray` 同步选择
5. 对局阶段 — `CGameMode` 管理对局，`StormCore` 管理胜利条件
6. 对局结束 — `MatchFinished()` 通知客户端，`TerminateSessionServer()` 清理

### 关键网络类

| 类 | 职责 |
|---|---|
| `UMGameInstance` | 登录、会话创建/搜索/加入、关卡加载 |
| `ATGameSession` | 玩家注册/注销，通知 GameInstance |
| `UTNetStatics` | 会话配置工具（端口、Coordinator URL、命令行参数） |
| `ACGameState` | 同步角色选择、击杀计数、击杀事件广播（Multicast） |
| `AMPlayerState` | 同步玩家选择（角色定义、槽位、队伍ID） |

### 网络同步策略

| 数据 | 同步方式 | 说明 |
|------|----------|------|
| 生命/法力/护甲等 | `COND_None` | 所有客户端需要（头顶血条） |
| 经验/金币/升级点 | `COND_OwnerOnly` | 只有本人需要 |
| 等级 | `COND_None` | 头顶 UI 显示 |
| 最大等级/最大经验 | `COND_InitialOnly` | 初始化后不变 |
| 伤害 Meta 属性 | 不同步 | 服务器计算后即清零 |
| 角色选择 | `GameState` Replicated | 大厅全员可见 |
| 击杀事件 | `Multicast RPC` | 全员播报 |
| 聊天消息 | `Client RPC` | 服务器转发到目标客户端 |
| 物品操作 | `Server RPC` → `Client RPC` | 服务器验证后通知客户端 |

## 游戏流程

```
主菜单 (MainMenuGameMode)
  │  登录 → 创建/加入房间
  ▼
大厅 (LobbyGameMode)
  │  角色选择 → CGameState::SetCharacterSelected()
  │  全员确认 → CGameState::CanStartMatch()
  ▼
对局 (CGameMode)
  │  SpawnDefaultPawnFor() 按选择生成英雄
  │  StormCore 管理推进
  │  MinionBarrack 周期刷新小兵
  │  击杀/助攻 → GAP_Dead 分配金币经验
  │  风暴核心到达目标 → GoalReached()
  ▼
结算 → 摄像机切换到风暴核心 → 返回主菜单
```

## 核心系统

### 角色系统

```
ACCharacter (基类)
├── IAbilitySystemInterface       → GAS 接口
├── IGenericTeamAgentInterface    → 队伍敌我判断
├── IRenderActorTargetInterface   → UI 3D渲染接口
├── UAbilitySystemComponent       → 技能系统组件
├── UOverHeadStatsGauge           → 头顶血条/蓝条
├── 死亡/重生/布娃娃              → Die(), Respawn()
└── 眩晕/击飞动画                 → Stunned(), GAP_Launched

    ACPlayerCharacter (玩家, 继承ACCharacter)
    ├── ICombatInterface          → 战斗交互
    ├── USpringArmComponent       → 摄像机臂
    ├── UCameraComponent          → 玩家摄像机
    ├── UMotionWarpingComponent   → 动画位移
    ├── UInventoryComponent       → 背包
    ├── UCHeroAttributeSet        → 英雄属性(经验/金币/等级)
    └── Enhanced Input System     → 输入

    AMinion (AI小兵, 继承ACCharacter)
    ├── 队伍皮肤切换
    └── 目标点/攻击目标
```

### AI 系统

| 类 | 职责 |
|---|---|
| `ACAIController` | 行为树 + AI 感知（视觉），队伍态度判断，死亡/眩晕时暂停 |
| `AMinion` | 小兵 Actor，队伍皮肤，目标分配 |
| `AMinionBarrack` | 小兵工厂，批量生成+对象池，可配置间隔和批次 |
| `UBTTask_SendInputToAbilitySystem` | BT 节点，AI 决策转化为技能输入 |

### 风暴核心

`AStormCore` 继承 `ACharacter`：
- `USphereComponent` 检测范围内角色，计算两队影响力权重
- 权重差决定核心移动方向和速度（`MaxMoveSpeed = 500`）
- 到达某队目标点时触发 `OnGoalReachedDelegate`，对局结束
- 有展开/捕获蒙太奇动画
- 提供 `ViewCam` 作为结算镜头

### 装备系统

详见 [API.md](API.md) 的装备系统章节。核心流程：

```
玩家点击商店物品
  → UInventoryComponent::TryPurchase()
  → Server_Purchase() [Server RPC, 带验证]
  → 检测是否可合成 (FindIngredientForItem)
  → 扣金币, 移除材料, GrantItem()
  → Client_ItemAdded() [Client RPC]
  → OnItemAdded 委托 → UI 更新
```

### UI 架构

**游戏内 UI** — UMG Widget，`ACPlayerController` 管理创建和显示切换。

**前端 UI** — CommonUI Widget Stack，`UFrontendUISubsystem`（GameInstance 子系统）管理。详见 [FRONTEND_UI_SYSTEM.md](FRONTEND_UI_SYSTEM.md)。

## 构建与部署

| Target | 类型 | 用途 |
|--------|------|------|
| `Crunch.Target.cs` | Game | 客户端打包 |
| `CrunchEditor.Target.cs` | Editor | 编辑器开发 |
| `CrunchServer.Target.cs` | Server | 专用服务器 |

```bash
# Docker 部署
cd ServerDeploy && docker-compose up -d

# 本地调试
launchScripts/launchServer.bat
launchScripts/launchGame.bat
```
