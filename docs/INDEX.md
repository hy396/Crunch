# Crunch 项目文档

## 文档列表

| 文档 | 内容 |
|------|------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | 系统架构、模块关系、网络拓扑、游戏流程 |
| [GAS_SYSTEM.md](GAS_SYSTEM.md) | Gameplay Ability System 实现详解 |
| [GAS_Mage_Abilities.md](GAS_Mage_Abilities.md) | 法师技能蓝图配置指南 |
| [GAS_Warrior_Abilities.md](GAS_Warrior_Abilities.md) | 战士技能蓝图配置指南 |
| [GAS_Optimization_Changelog.md](GAS_Optimization_Changelog.md) | GAS 性能优化变更记录 |
| [FRONTEND_UI_SYSTEM.md](FRONTEND_UI_SYSTEM.md) | CommonUI 前端框架（选项、键位） |
| [CHAT_SYSTEM.md](CHAT_SYSTEM.md) | 聊天系统（频道、弹幕、RPC） |
| [API.md](API.md) | 核心类 API 参考 |
| [DEVELOPMENT.md](DEVELOPMENT.md) | 开发环境、编码规范、调试方法 |
| [COPYRIGHT.md](COPYRIGHT.md) | 版权与许可信息 |

## 快速导航

**新加入项目？** 先读 [ARCHITECTURE.md](ARCHITECTURE.md) 了解整体结构，再根据你负责的模块深入阅读。

**要添加新技能？** 参考 [GAS_SYSTEM.md](GAS_SYSTEM.md) 了解技能框架，然后看 [GAS_Mage_Abilities.md](GAS_Mage_Abilities.md) 或 [GAS_Warrior_Abilities.md](GAS_Warrior_Abilities.md) 的蓝图配置流程。

**要做 UI？** 游戏内 HUD 直接用 UMG；前端界面（菜单、选项）用 CommonUI 框架，详见 [FRONTEND_UI_SYSTEM.md](FRONTEND_UI_SYSTEM.md)。

**要改网络逻辑？** [ARCHITECTURE.md](ARCHITECTURE.md) 的网络架构章节 + [API.md](API.md) 的 RPC 接口。

## 目录结构速查

```
Source/Crunch/Private/
├── AI/            CAIController, Minion, MinionBarrack
├── Actor/         ProjectileActor 及其子类, ArcaneTrapActor, BurningGroundActor
├── Animations/    CAnimInstance, AnimNotify (攻击窗口/事件/目标组)
├── Character/     CCharacter(基类), CPlayerCharacter, PDA_CharacterDefinition
├── Framework/     CGameMode, CGameState, MGameInstance, LobbyGameMode, StormCore
├── FrontendUI/    CommonUI 前端框架 (选项/键位绑定/确认弹窗)
├── GAS/
│   ├── Abilities/ 所有 GA_* 技能 + GAP_* 被动
│   ├── Core/      ASC, 属性集, 标签, 基类, 调试工具
│   ├── Data/      PDA_AbilitySystemGenerics
│   ├── Executions/ECC_AttackDamage (伤害执行计算)
│   ├── MMC/       自定义幅度计算 (冷却/等级缩放)
│   └── TA/        目标Actor (圆形/线形/黑洞/地面选点)
├── Inventory/     InventoryComponent, InventoryItem, PDA_ShopItem
├── Network/       TGameSession, TNetStatics
├── Player/        CPlayerController, MPlayerState, LobbyPlayerController
└── UI/
    ├── Ability/   技能栏, 技能提示
    ├── Chat/      聊天系统
    ├── Common/    十字准心, 3D渲染, 物品提示
    ├── Gameplay/  战斗HUD, 属性面板, 伤害数字, 经验条
    ├── Inventory/ 背包界面, 拖拽
    ├── KillFeed/  击杀播报
    ├── Lobby/     角色选择, 队伍编排
    ├── MainMenu/  主菜单, 房间列表
    ├── Minimap/   小地图
    ├── Portrait/  队友头像状态
    ├── Shop/      商店, 合成树
    └── StatusEffect/ 状态效果图标
```
