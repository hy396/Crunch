# 🏰 MOBA

> UE 5.4 + GAS 驱动的 **5v5 MOBA 多人对战游戏**：双方选择英雄，在对称地图上推进**风暴核心**冲入敌方基地即获胜。包含完整 C++ 源码、专用服务器、自建 Python 协调器与 Docker 部署。

![UE](https://img.shields.io/badge/Unreal_Engine-5.4-0D47A1?logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C)
![GAS](https://img.shields.io/badge/Combat-GameplayAbilitySystem-00A86B)
![Network](https://img.shields.io/badge/Network-DedicatedServer-E4405F)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows)

---

## 📑 目录

- [🎮 游戏概览](#-游戏概览)
- [🕐 创建时间](#-创建时间)
- [✨ 项目亮点](#-项目亮点)
- [🛠 技术栈](#-技术栈)
- [🧱 核心系统](#-核心系统)
- [🚀 当前进度](#-当前进度)
- [🏃 运行](#-运行)
- [📁 目录结构](#-目录结构)
- [📚 文档](#-文档)

---

## 🎮 游戏概览

5v5 MOBA 多人对战：两支队伍在对称地图上争夺**风暴核心**，把核心推入对方基地的一方获胜。

- ⚔️ **3 个可玩英雄**（13 个技能）：Crunch / Kisara / Reimu，全部由 GAS 驱动
- 🗺 **完整对局链路**：登录 → 创建/搜索房间 → 大厅选人 → 对局 → 结算
- 🛡 **小兵 AI**：BehaviorTree + AI Perception 驱动兵线
- 🏪 **商店系统**：物品数据资产 + 服务端校验的购买/合成流程
- 🌐 **多人网络**：专用服务器 + EOS 会话 + Python 协调器分配端口 + Docker 部署

## 🕐 创建时间

2025-06 开始开发（持续迭代中，已经弃坑）

## ✨ 项目亮点

1. **5v5 多人对局全链路** — 从登录、房间会话（EOS OnlineSubsystem）到大厅选人、对局、结算的完整网络流程，`CrunchServer` 独立专用服务器构建目标
2. **GAS 全栈驱动战斗** — 3 英雄 13 个技能全部由 GameplayAbility + GameplayEffect + AnimMontage + TargetActor 组成，伤害计算走 GameplayEffect 管道（Executions / MMC）
3. **自建 Python 协调器 + Docker 部署** — `Coordinator`（HTTP 分配端口）编排多服务器实例，`ServerDeploy/docker-compose` 一键部署生产环境
4. **数据驱动内容** — 英雄用 `DA_CharacterDefinition` 数据资产定义、商店物品用 `PDA_ShopItem` 资产配置，核心系统与游戏内容解耦
5. **完整工程化配套** — 单模块 Private-only 分层（GAS/Framework/Character/AI/Network），配套 Wiki 设计文档 + Docs 技术文档双体系

## 🛠 技术栈

| 类别 | 技术                                                                     |
| ---- | ------------------------------------------------------------------------ |
| 引擎 | UE 5.4（客户端 / 编辑器 /**专用服务器**三构建目标）                |
| 战斗 | Gameplay Ability System（GA / GE / AttributeSet / Executions / MMC）     |
| 网络 | Dedicated Server + EOS OnlineSubsystem + Python Coordinator（HTTP）      |
| 动画 | MotionWarping + AnimNotify / AnimNotifyState（伤害窗口 + GameplayEvent） |
| AI   | BehaviorTree + AI Perception                                             |
| UI   | UMG（游戏内 HUD）+ CommonUI Widget Stack（前端菜单）                     |
| 部署 | Docker / docker-compose（`ServerDeploy/`）                             |

## 🧱 核心系统

| 系统     | 位置                               | 关键类                                                                       |
| -------- | ---------------------------------- | ---------------------------------------------------------------------------- |
| GAS 技能 | `GAS/`                           | `UCAbilitySystemComponent` / `UCGameplayAbility` / `UCAttributeSet`    |
| 角色     | `Character/` `Player/` `AI/` | `ACCharacter` / `ACPlayerCharacter` / `AMinion` / `ICombatInterface` |
| 框架     | `Framework/`                     | `CGameMode` / `CGameState` / `AStormCore` / `LobbyGameMode`          |
| 装备     | `Inventory/`                     | `UInventoryComponent` / `UPDA_ShopItem`                                  |
| 网络     | `Network/` + `Coordinator/`    | `ATGameSession` / `UTNetStatics` / `coordinator.py`                    |
| UI       | `UI/` `FrontendUI/`            | `UFrontendUISubsystem` / `UWidget_PrimaryLayout`                         |

## 🚀 当前进度

| 模块                                                            | 状态                              |
| --------------------------------------------------------------- | --------------------------------- |
| 框架层（GameMode/GameState/GameInstance/大厅-对局链路）         | ✅ 已实现                         |
| 英雄：Crunch（5 技能）/ Kisara（5 技能）/ Reimu（3 技能）       | ✅ 可玩（GA+GE+Montage 齐全）     |
| 小兵 AI（BehaviorTree + 兵营）                                  | ✅ 已实现                         |
| 风暴核心 / 商店 / 物品（17 项数据资产）                         | ✅ 已实现                         |
| 大厅 / 主菜单 / CommonUI 前端框架                               | ✅ 已实现                         |
| 其余技能类（`GA_ArcaneOrb` 等仅 C++ 占位）、法师/大斧战士英雄 | ⏳ 规划中（见`Wiki/Heroes.md`） |

## 🏃 运行

```bash
# 环境：UE 5.4 + VS2022；DS + 客户端双终端本地运行：
set UNREAL_EDITOR=<UE根目录>
launchScripts/launchServer.bat   # 终端1：DS 监听 7779
launchScripts/launchGame.bat     # 终端2：客户端连接 127.0.0.1:7779
```

- 编辑器 2 人测试：PIE → Advanced Settings → Number of Players = 2+，Net Mode = **Play As Listen Server**
- 生产部署：`cd ServerDeploy && docker-compose up -d`（详见 `Docs/DEPLOYMENT.md`）
- ⚠️ **VRM4U 插件**需自行构建 `assimp-vc141-mt.lib` 放入 ThirdParty 目录后编译

## 📁 目录结构

```
Crunch/
├── Source/Crunch/Private/    # 单主模块（Private-only）
│   ├── GAS/                  # 技能（Abilities/Core/Executions/MMC/TargetActor）
│   ├── Framework/            # GameMode/GameState/GameInstance/StormCore
│   ├── Character/ Player/    # 角色基类 + 玩家派生
│   ├── AI/                   # CAIController/Minion/MinionBarrack
│   ├── Actor/ Animations/    # 投射物/陷阱 + 动画通知
│   ├── Inventory/ Network/   # 背包商店 + 会话
│   └── UI/ FrontendUI/       # 游戏内 HUD + CommonUI 前端
├── Coordinator/              # Python 会话分配服务
├── ServerDeploy/             # Docker 镜像与 compose
├── Wiki/                     # 游戏设计文档（玩法/英雄/机制/经济）
├── Docs/                     # 技术文档（架构/GAS/网络/UI/部署）
└── Content/ Config/ Plugins/
```

## 📚 文档

- 游戏设计、机制详解：[`Wiki/Home.md`](Wiki/Home.md)
- 系统架构、网络拓扑、GAS 管线：[`Docs/INDEX.md`](Docs/INDEX.md)

## 📄 许可

代码遵循 MIT；第三方插件（VRM4U、KawaiiPhysics 等）与部分资源遵循各自许可，见 [`Docs/COPYRIGHT.m`](Docs/COPYRIGHT.md)

# Crunch

一款基于 Unreal Engine 5.4 的 5v5 MOBA 多人对战游戏。两支队伍选择英雄，在对称地图上推进**风暴核心**——把核心推入对方基地的一方获胜。

本仓库包含游戏的完整 C++ 源码、蓝图资产、网络服务器与协调器（Coordinator），以及配套的 Wiki 与技术文档。

## 技术栈

- **引擎**：Unreal Engine 5.4（`EngineAssociation` GUID 见 `Crunch.uproject`）
- **核心系统**：Gameplay Ability System（GAS）驱动所有战斗逻辑
- **网络**：自建 Python Coordinator（HTTP 分配端口）+ EOS OnlineSubsystem 做会话发现与大厅
- **UI**：游戏内 HUD 用 UMG；前端菜单/选项用 CommonUI Widget Stack
- **动画**：MotionWarping 驱动技能位移，AnimNotify / AnimNotifyState 控制伤害窗口与 GameplayEvent 触发
- **AI**：BehaviorTree + AI Perception 驱动小兵
- **C++ 模块**：单主模块 `Crunch`（`Source/Crunch/`），Private-only 布局
- **构建目标**：`Crunch`（客户端）、`CrunchEditor`（编辑器）、`CrunchServer`（专用服务器）

## 快速开始

### 环境要求

- UE 5.4（通过 Epic Launcher 安装）
- Visual Studio 2022 + “使用 C++ 的游戏开发”工作负载
- Git

> ⚠️ **VRM4U 插件**需要额外的 `assimp-vc141-mt.lib`，仓库里没有。从 [ruyo/assimp](https://github.com/ruyo/assimp) 自行 CMake 构建 Release，将 `.lib`/`.dll` 放入 `Plugins/VRM4U/ThirdParty/assimp/lib/x64/Release/` 与 `bin/x64/` 后再编译。

### 构建

```bash
# 编辑器（开发主力）
"<UE5.4>/Engine/Build/BatchFiles/Build.bat" CrunchEditor Win64 Development -project="$PWD/Crunch.uproject"

# 专用服务器
"<UE5.4>/Engine/Build/BatchFiles/Build.bat" CrunchServer Win64 Development -project="$PWD/Crunch.uproject"

# 客户端（打包用）
"<UE5.4>/Engine/Build/BatchFiles/Build.bat" Crunch Win64 Development -project="$PWD/Crunch.uproject"
```

或直接用 VS/Rider 打开 `Crunch.sln`，将 `CrunchEditor` 设为启动项目后 F5。

### 本地运行（专用服务器 + 客户端）

需要先设置环境变量 `UNREAL_EDITOR` 指向 UE 根目录。两个终端：

```bash
launchScripts/launchServer.bat   # 终端1：DS，监听 7779
launchScripts/launchGame.bat     # 终端2：客户端，连接 127.0.0.1:7779
```

WSL 环境可使用 `launchScripts/wsl_S.bat`。

编辑器内 2 人测试：PIE → Advanced Settings → Number of Players = 2+，Net Mode = Play As Listen Server。

### 部署

生产部署走 Docker，详见 `Docs/DEPLOYMENT.md`：

```bash
cd ServerDeploy
docker-compose up -d
```

## 项目结构

```text
Crunch/
├── Crunch.uproject              # 项目入口（引擎版本 + 模块/插件声明）
├── Source/Crunch/               # 唯一主模块（Private-only 布局）
│   ├── Crunch.Build.cs          # 依赖：GAS、OnlineSubsystemEOS、CommonUI...
│   └── Private/
│       ├── GAS/                 # 技能系统（Abilities/Core/Data/Executions/MMC/TA）
│       ├── Framework/           # GameMode/GameState/GameInstance/StormCore
│       ├── Character/           # ACCharacter 基类 + Player/AI 派生
│       ├── Player/              # PlayerController、PlayerState、大厅控制器
│       ├── AI/                  # CAIController、Minion、MinionBarrack
│       ├── Actor/               # 投射物、陷阱、地面效果
│       ├── Animations/          # AnimInstance、AnimNotify(State)
│       ├── Inventory/           # 背包组件 + 商店物品数据资产
│       ├── Network/             # GameSession、NetStatics
│       ├── UI/                  # 游戏内 UMG Widget（按特性分子目录）
│       └── FrontendUI/          # CommonUI 前端（菜单/选项/键位）
├── Content/                     # 蓝图资产、地图、UI、音频、VFX
├── Config/                      # DefaultEngine/Game/Input/Tags
├── Plugins/                     # VRM4U、KawaiiPhysics、RiderLink
├── Coordinator/                 # Python 会话分配服务（coordinator.py）
├── ServerDeploy/                # Docker 镜像与 compose
├── Wiki/                        # 游戏设计文档（玩法、英雄、机制）
└── Docs/                        # 技术文档（架构、GAS、网络、UI、部署）
```

## 核心系统一览

| 系统     | 实现位置                             | 关键类                                                                                                                |
| -------- | ------------------------------------ | --------------------------------------------------------------------------------------------------------------------- |
| GAS 技能 | `GAS/`                             | `UCAbilitySystemComponent`, `UCGameplayAbility`, `UCAttributeSet`, `UCHeroAttributeSet`, `ECC_AttackDamage` |
| 角色     | `Character/`, `Player/`, `AI/` | `ACCharacter`, `ACPlayerCharacter`, `AMinion`, `ICombatInterface`                                             |
| 框架     | `Framework/`                       | `CGameMode`, `CGameState`, `MGameInstance`, `LobbyGameMode`, `AStormCore`                                   |
| 装备     | `Inventory/`                       | `UInventoryComponent`, `UPDA_ShopItem`, `UInventoryItem`                                                        |
| 网络     | `Network/` + `Coordinator/`      | `ATGameSession`, `UTNetStatics`, `coordinator.py`                                                               |
| UI       | `UI/`, `FrontendUI/`             | `UFrontendUISubsystem`, `UWidget_PrimaryLayout`                                                                   |

## 当前已实现内容

以下每一项都在 `Content/` 下有对应的蓝图 / 数据资产，不是仅有 C++ 类。

### 框架层

- **GameMode / GameState / GameInstance**：`BP_CGameMode`、`BP_GameState`、`BP_GameInstance` 均已配置。
- **Player 侧**：`BP_CPlayerController`、`BP_PlayerState`、`BP_UNumberPopComponent_NiagaraText`。
- **大厅到对局的链路**：登录 → 创建/搜索房间 → 大厅选人 → 对局 → 结算。

### 英雄与角色

- **Crunch**：5 个技能（Combo / Dash / GroundBlast / Tornado / UpperCut）均有 `GA_*` BP、GameplayEffect、AnimMontage。
- **Kisara**：5 个技能（ChainAttack / Combo / Dash / SwordAura / UpperCut）均有 `GA_*` BP 与 AnimMontage。
- **Reimu**：3 个技能（Blackhole / Blink / Freeze）均有 `GA_*` BP、GameplayEffect、AnimMontage、TargetActor。
- 三个英雄均有对应的 `DA_CharacterDefinition_*.uasset` 数据资产。

> `Source/Crunch/Private/GAS/Abilities/` 下还有大量仅有 C++、未配蓝图 / 数据资产的技能类（`GA_ArcaneOrb`、`GA_Blink`、`GA_MeteorStrike`、`GA_ArcaneStorm`、`GA_WarCry`、`GA_Guillotine`、`GA_AxeThrow`、`GA_Earthquake` 等），以及 `GAP_Dead`、`GAP_Launched` 等被动——它们是占位实现，目前没有任何英雄 / 数据资产引用，**不在「已实现」范围内**。`Wiki/Heroes.md` 中描述的「法师 / 大斧战士」同样仅停留在设计文档阶段。

### AI 与小兵

- **小兵 BP**：`BP_Minion`、`BP_MinionBarrack`、`BP_AIC_Minion`。
- **行为树**：`BT_Minion`、`BB_Minion`、`ABP_Minion`、`AM_Minion_Death`、`AM_Minion_Stun`。
- **小兵技能**：`GA_Combo_Minion` + 对应 GE。
- **初始化**：`GE_Init_Minion`。

### 风暴核心

- `BP_StormCore`（带 `ViewCam` 作为结算镜头），地面贴花 + 材质已配置。

### 商店与物品

- **商店 UI**：`WBP_Shop`、`WBP_ShopItem`、`IA_ToggleShop`。
- **已配物品数据资产**：`HealthPack`、`Vanguard`、`BootsOfCharges`、`PhaseShield`、`RingOfPower`、`RingOfVoid`、`RingOfLengend`、`PotionOfPurity`、`Biji`、`Shifu`、`MingwenJujian`、`KuangbaoJujian`、`Caonijian`、`Wuduzhang`、`ShenqiNaifu`、`Huimiemozhang`、`Goddess_Wisdom`。
- **服务端校验的购买 / 合成流程**在 `InventoryComponent` 中实现。

### UI

- **HUD**：`W_Healthbar`、`W_Manabar2` + 配套材质 / 字体。
- **大厅**：`WBP_Lobby`、`WBP_AbilityGauge_Lobby`、`WBP_PlayerTeamLayout`、`WBP_PlayerTeamSlot`、`WBP_TeamSelectionWidget`。
- **主菜单**：`WBP_MainMenu` + SessionEntry 子项。
- **前端框架**：`WBP_Template_Layout`（CommonUI Widget Stack）+ `DA_DataListEntryMapping`。
- **通用资产**：UI 字体（Orbitron、NotoSans 多语言变体）、HUD / Menu 的样式与图标材质。

## 扩展阅读

- 游戏设计、机制详解：见 [`Wiki/Home.md`](Wiki/Home.md)
- 系统架构、网络拓扑、GAS 管线：见 [`Docs/INDEX.md`](Docs/INDEX.md)

## 许可

本项目代码遵循 MIT 许可证，详见 [LICENSE](LICENSE)。项目中包含的第三方插件（VRM4U、KawaiiPhysics、RiderLink 等）以及部分资源遵循各自的原始许可协议——使用前请阅读 [Docs/COPYRIGHT.md](Docs/COPYRIGHT.md)。
