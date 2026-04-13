# Wiki

欢迎来到 Crunch 项目 Wiki。一款基于 Unreal Engine 5.4 的 5v5 MOBA 游戏。

## 游戏设计

- [游戏概述](Game-Overview.md) - 玩法规则、胜利条件、游戏流程
- [英雄列表](Heroes.md) - 所有可选英雄及定位
  - [法师](Hero-Mage.md) - 技能详解
  - [大斧战士](Hero-Warrior.md) - 技能详解
- [属性系统](Attributes.md) - 生命、法力、攻击、防御等属性
- [伤害计算](Damage-Formula.md) - 伤害公式、穿透、减免机制
- [装备系统](Items.md) - 商店、合成、物品效果
- [风暴核心](Storm-Core.md) - 核心目标争夺机制
- [小兵系统](Minions.md) - 小兵刷新、AI 行为
- [经济系统](Economy.md) - 金币获取、击杀赏金

## 开发 Walkthrough

- [添加新英雄技能](Walkthrough-New-Hero.md) - 从 C++ 类到蓝图配置的完整流程
- [添加新装备](Walkthrough-New-Item.md) - 创建数据资产到测试验证

## Troubleshooting

- [常见问题排查](Troubleshooting.md) - 编译错误、API 不兼容、网络调试

## 技术文档 (docs/)

详细的技术参考文档位于 [docs/](../docs/) 目录：

- [ARCHITECTURE.md](../docs/ARCHITECTURE.md) - 系统架构、模块关系、网络拓扑
- [GAS_SYSTEM.md](../docs/GAS_SYSTEM.md) - GAS 技能系统详解
- [API.md](../docs/API.md) - 核心类 API 参考
- [FRONTEND_UI_SYSTEM.md](../docs/FRONTEND_UI_SYSTEM.md) - CommonUI 前端框架
- [CHAT_SYSTEM.md](../docs/CHAT_SYSTEM.md) - 聊天系统
- [DEVELOPMENT.md](../docs/DEVELOPMENT.md) - 开发环境、编码规范
- [GAS_Mage_Abilities.md](../docs/GAS_Mage_Abilities.md) - 法师蓝图配置指南
- [GAS_Warrior_Abilities.md](../docs/GAS_Warrior_Abilities.md) - 战士蓝图配置指南
- [GAS_Optimization_Changelog.md](../docs/GAS_Optimization_Changelog.md) - GAS 性能优化记录
