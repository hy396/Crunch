# Crunch

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.4-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-red)
![License](https://img.shields.io/badge/License-MIT-green)

Crunch 是一款基于 Unreal Engine 5.4 的多人在线竞技场（MOBA）游戏。两支队伍各自选择英雄，通过技能对抗、击杀小兵、购买装备来积累优势，最终推进并夺取对方的风暴核心（Storm Core）获得胜利。

## 核心玩法

- **英雄对战** — 多角色可选，每个英雄拥有 4 个主动技能（QERF）+ 普攻连段
- **风暴核心** — 地图中央的核心目标，队伍在范围内施加影响力来争夺控制权
- **装备系统** — 金币经济 + 商店购买 + 装备合成树
- **小兵与推进** — AI 小兵从兵营周期性刷新，沿路线向敌方推进
- **团队协作** — 队内实时聊天、小地图、击杀播报、队友状态显示

## 已实现英雄技能

### 通用
| 技能 | 类型 | 说明 |
|------|------|------|
| GA_Combo | 普攻 | 多段连击 |
| GA_Shoot | 远程 | 远程射击 |
| GA_ChainAttack | 连锁 | 多目标连续攻击 |
| GA_Dash | 位移 | 冲刺 |
| GA_Blink | 位移 | 闪现 |
| GA_Freeze | 控制 | 冰冻 |
| GA_Tornado | 控制 | 龙卷风 |
| GA_BlackHole | 控制 | 黑洞吸引 + 持续伤害 |
| GA_GroundBlast | 伤害 | 地面冲击 |
| GA_Laser | 伤害 | 激光 |
| GA_SwordAura | 伤害 | 剑气 |
| UpperCut | 伤害 | 上挑 |

### 法师
| 技能 | 按键 | 说明 |
|------|------|------|
| GA_ArcaneOrb | Q | 穿透奥术球，距离越远伤害越高，命中回蓝 |
| GA_PhaseShift | E | 无敌后撤 + 原地放置奥术陷阱（定身） |
| GA_MeteorStrike | F | 地面选点陨石打击 + 燃烧地面 DOT |
| GA_ArcaneStorm | R | 持续 AOE 减速 + 最终爆发沉默（大招） |

### 大斧战士
| 技能 | 按键 | 说明 |
|------|------|------|
| GA_WarCry | Q | AOE 伤害 + 减甲减速 |
| GA_Guillotine | E | 锥形劈砍 + 低血量斩杀加成 + 流血 |
| GA_AxeThrow | F | 回旋飞斧，去程回程双段伤害 |
| GA_Earthquake | R | 三段砸地，前两段眩晕，第三段击飞（大招） |

### 被动
| 技能 | 说明 |
|------|------|
| GAP_Dead | 死亡奖励分配（击杀/助攻金币 + 连杀连败机制） |
| GAP_Launched | 击飞状态处理 |

## 技术架构

```
                  ┌──────────────────┐
                  │   Coordinator    │
                  │    (Python)      │
                  │  会话调度/负载均衡 │
                  └────────┬─────────┘
                           │ HTTP
            ┌──────────────┼──────────────┐
            ▼              ▼              ▼
   ┌────────────┐  ┌────────────┐  ┌────────────┐
   │  DS Server │  │  DS Server │  │  DS Server │
   │  (UE5)     │  │  (UE5)     │  │  (UE5)     │
   └──────┬─────┘  └──────┬─────┘  └──────┬─────┘
          │ Replication    │               │
     ┌────┴────┐      ┌───┴───┐       ┌───┴───┐
     │ Clients │      │Clients│       │Clients│
     └─────────┘      └───────┘       └───────┘
```

### 核心技术栈
| 层面 | 技术 |
|------|------|
| 引擎 | Unreal Engine 5.4 |
| 技能系统 | Gameplay Ability System (GAS) |
| UI 框架 | CommonUI + UMG |
| 在线服务 | Epic Online Services (EOS) |
| 输入系统 | Enhanced Input System |
| AI | Behavior Tree + AI Perception |
| 动画 | Motion Warping + KawaiiPhysics |
| 角色模型 | VRM4U（VRM 格式支持） |
| 部署 | Docker + Python Coordinator |

### GAS 架构

```
CAbilitySystemComponent (扩展ASC)
├── CGameplayAbility (技能基类)
│   ├── 伤害管线: MakeDamageEffectContext → ApplyDamageSpecToTarget
│   ├── 推力系统: PushCharacterFromLocation
│   └── 所有 GA_* 技能继承此类
├── CAttributeSet (基础属性集)
│   ├── Health/MaxHealth, Mana/MaxMana
│   ├── AttackPower, MagicPower, Armor, MagicResistance
│   ├── AttackDamage/MagicDamage/TrueDamage (Meta属性，不同步)
│   └── MoveSpeed, ArmorPen, MagicPen, DamageAmp/Reduction
├── CHeroAttributeSet (英雄属性集)
│   └── Experience, Level, Gold, UpgradePoint
├── ECC_AttackDamage (伤害执行计算)
│   └── 物理穿透 → 护甲减免 → 伤害加深/减免 → 最终伤害
├── TargetActor (目标检测)
│   ├── TargetActor_Around (圆形范围)
│   ├── TargetActor_Line (线性/锥形)
│   ├── TargetActor_BlackHole (黑洞吸引)
│   └── TargetActor_GroundPick (地面选点)
└── TGameplayTags (统一标签管理)
```

## 项目结构

```
Crunch/
├── Source/
│   ├── Crunch/
│   │   ├── Crunch.Build.cs            # 模块依赖配置
│   │   └── Private/
│   │       ├── AI/                     # AI控制器、小兵、兵营
│   │       ├── Actor/                  # 投射物、陷阱、燃烧地面等Actor
│   │       ├── Animations/             # 动画实例、AnimNotify
│   │       ├── Character/              # 角色基类、玩家角色、角色数据定义
│   │       ├── Framework/              # GameMode/State/Instance、风暴核心
│   │       ├── FrontendUI/             # CommonUI前端系统（选项、键位绑定）
│   │       ├── GAS/                    # 技能系统
│   │       │   ├── Abilities/          # 所有技能实现
│   │       │   ├── Core/               # ASC、属性集、标签、调试工具
│   │       │   ├── Data/               # 技能数据资产
│   │       │   ├── Executions/         # 伤害执行计算
│   │       │   ├── MMC/                # 自定义幅度计算
│   │       │   └── TA/                 # 目标Actor
│   │       ├── Inventory/              # 背包、物品、商店数据
│   │       ├── Network/                # 会话管理、网络工具
│   │       ├── Player/                 # 玩家控制器、PlayerState
│   │       └── UI/                     # 游戏内UI
│   │           ├── Ability/            # 技能栏、技能提示
│   │           ├── Chat/               # 聊天系统
│   │           ├── Common/             # 十字准心、物品提示、3D渲染
│   │           ├── Gameplay/           # 战斗HUD、属性面板
│   │           ├── Inventory/          # 背包界面、拖拽
│   │           ├── KillFeed/           # 击杀播报
│   │           ├── Lobby/              # 角色选择、队伍编排
│   │           ├── MainMenu/           # 主菜单、房间列表
│   │           ├── Minimap/            # 小地图
│   │           ├── Portrait/           # 队友头像状态
│   │           ├── Shop/               # 商店、合成树
│   │           └── StatusEffect/       # 状态效果图标
│   ├── Crunch.Target.cs                # 客户端构建
│   ├── CrunchEditor.Target.cs          # 编辑器构建
│   └── CrunchServer.Target.cs          # 专用服务器构建
├── Plugins/
│   ├── VRM4U/                          # VRM 模型导入支持
│   ├── KawaiiPhysics/                  # 物理弹簧骨骼动画
│   └── Developer/RiderLink/            # Rider IDE 集成
├── Config/                             # 引擎/输入/标签配置
├── Content/                            # 蓝图、资产、地图
├── ServerDeploy/                       # Docker 部署配置
│   ├── server/                         # 游戏服务器镜像
│   ├── coordinator/                    # 协调器镜像
│   └── docker-compose.yaml
├── Coordinator/                        # Python 协调器源码
│   ├── coordinator.py
│   └── consts.py
├── launchScripts/                      # 本地启动脚本
└── docs/                               # 项目文档
```

## 快速开始

### 环境要求

- Windows 10/11
- Unreal Engine 5.4
- Visual Studio 2022+（需安装 C++ 游戏开发工作负载）
- 16GB+ RAM

### 构建步骤

1. 克隆仓库并确保 UE 5.4 已安装

2. 右键 `Crunch.uproject` → Generate Visual Studio project files

3. 打开 `Crunch.sln`，设置 `CrunchEditor` 为启动项目，编译运行

4. 或通过命令行构建：
   ```bash
   # 编辑器版本
   Engine/Build/BatchFiles/Build.bat CrunchEditor Win64 Development -project="<路径>/Crunch.uproject"

   # 专用服务器
   Engine/Build/BatchFiles/Build.bat CrunchServer Win64 Development -project="<路径>/Crunch.uproject"
   ```

### 本地测试

```bash
# 启动服务器
launchScripts/launchServer.bat

# 启动客户端
launchScripts/launchGame.bat
```

### Docker 部署

```bash
cd ServerDeploy
docker-compose up -d
```

## 网络同步设计

| 数据 | 同步策略 | 说明 |
|------|----------|------|
| 生命/法力/护甲等 | `COND_None` | 所有客户端都需要（头顶血条） |
| 经验/金币/升级点 | `COND_OwnerOnly` | 只有本人需要 |
| 等级 | `COND_None` | 所有人需要看到（头顶UI） |
| 最大等级/最大经验 | `COND_InitialOnly` | 初始化后不变 |
| 伤害 Meta 属性 | 不同步 | 服务器计算后即清零 |
| 角色选择 | `GameState` 广播 | 大厅阶段全员可见 |
| 击杀事件 | `Multicast RPC` | 全员播报 |
| 聊天消息 | `Client RPC` | 服务器转发到目标客户端 |

## 第三方插件

| 插件 | 用途 |
|------|------|
| [VRM4U](https://github.com/ruyo/VRM4U) | VRM 格式角色模型导入和运行时加载 |
| [KawaiiPhysics](https://github.com/pafuhana1213/KawaiiPhysics) | 头发、衣物等弹簧骨骼物理动画 |
| [AsyncLoadingScreen](https://github.com/truong-bui/AsyncLoadingScreen) | 异步加载界面 |
| Epic Online Services (EOS) | 在线会话、语音聊天 |
| GameplayAbilities | 技能系统框架 |
| CommonUI | 现代化 UI 框架（Widget Stack、输入路由） |
| MotionWarping | 动画驱动的位移（技能位移贴合目标） |

## 文档

详细文档位于 [docs/](docs/) 目录：

| 文档 | 内容 |
|------|------|
| [INDEX.md](docs/INDEX.md) | 文档索引 |
| [ARCHITECTURE.md](docs/ARCHITECTURE.md) | 系统架构 |
| [GAS_SYSTEM.md](docs/GAS_SYSTEM.md) | GAS 技能系统详解 |
| [GAS_Mage_Abilities.md](docs/GAS_Mage_Abilities.md) | 法师技能蓝图配置指南 |
| [GAS_Warrior_Abilities.md](docs/GAS_Warrior_Abilities.md) | 战士技能蓝图配置指南 |
| [GAS_Optimization_Changelog.md](docs/GAS_Optimization_Changelog.md) | GAS 性能优化记录 |
| [FRONTEND_UI_SYSTEM.md](docs/FRONTEND_UI_SYSTEM.md) | CommonUI 前端框架 |
| [CHAT_SYSTEM.md](docs/CHAT_SYSTEM.md) | 聊天系统 |
| [DEPLOYMENT.md](docs/DEPLOYMENT.md) | 部署指南 |
| [DEVELOPMENT.md](docs/DEVELOPMENT.md) | 开发环境 |
| [API.md](docs/API.md) | 核心类 API |

## 许可证

代码部分采用 [MIT License](LICENSE)。项目中使用的第三方资源（模型、纹理、音频等）遵循各自的版权协议，Unreal Engine 相关资源遵循 Epic Games 许可协议。
