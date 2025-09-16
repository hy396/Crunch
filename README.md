# Crunch - UE5多人游戏项目

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.4-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-red)
![License](https://img.shields.io/badge/License-Epic%20Games-green)

## 📖 项目简介

**Crunch** 是一个基于Unreal Engine 5.4开发的多人在线对战游戏。游戏支持团队对抗模式，玩家可以选择不同的角色进行战斗，通过技能升级和物品合成来增强实力。

### 🎮 核心特性

- **多人对战** - 支持团队对抗模式，实时网络同步
- **角色系统** - 多种可选角色，每个角色具有独特的技能和特性
- **技能树** - 基于GAS系统的技能升级和能力系统
- **物品系统** - 完整的商店、库存和物品合成系统
- **实时UI** - 丰富的游戏界面，包括角色选择、游戏内HUD等
- **连锁攻击技能** - 新增连锁攻击技能，可对多个目标进行连续攻击
- **弹幕聊天系统** - 支持实时滚动的弹幕聊天效果
- **智能合成系统** - 改进的物品合成机制，支持智能材料查找

## 🏗️ 技术架构

### 核心技术栈
- **游戏引擎**: Unreal Engine 5.4
- **编程语言**: C++17, Blueprint
- **网络架构**: UE5 Replication System
- **技能系统**: Gameplay Ability System (GAS)
- **UI框架**: UMG (Unreal Motion Graphics)
- **部署**: Docker容器化

### 系统架构图
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Game Client   │    │   Game Server   │    │   Coordinator   │
│                 │    │                 │    │    (Python)     │
│ - UI System     │◄──►│ - Match Logic   │◄──►│ - Server Mgmt   │
│ - Input Handling│    │ - Network Sync  │    │ - Load Balance  │
│ - Local Preview │    │ - Game State    │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 📁 项目结构

```
Crunch/
├── 📂 Source/Crunch/              # 核心游戏代码
│   ├── 📂 Private/
│   │   ├── 📂 AI/                 # AI系统
│   │   ├── 📂 Actor/              # 游戏Actor
│   │   ├── 📂 Animations/         # 动画系统
│   │   ├── 📂 Character/          # 角色系统
│   │   ├── 📂 Framework/          # 游戏框架
│   │   ├── 📂 GAS/                # 技能系统(GAS)
│   │   ├── 📂 Inventory/          # 库存系统
│   │   ├── 📂 Network/            # 网络系统
│   │   ├── 📂 Player/             # 玩家系统
│   │   └── 📂 UI/                 # 用户界面
│   │       ├── 📂 Common/         # 通用UI组件
│   │       ├── 📂 Gameplay/       # 游戏内UI
│   │       ├── 📂 Lobby/          # 大厅UI
│   │       └── 📂 MainMenu/       # 主菜单UI
│   └── 📂 Public/                 # 公共头文件
├── 📂 Config/                     # 游戏配置文件
├── 📂 Plugins/                    # 第三方插件
│   ├── 📂 VRM4U/                  # VRM模型支持
│   ├── 📂 KawaiiPhysics/          # 物理动画
│   └── 📂 RiderLink/              # IDE集成
├── 📂 ServerDeploy/               # 服务器部署
└── 📂 launchScripts/              # 启动脚本
```

## 🚀 快速开始

### 环境要求

- **操作系统**: Windows 10/11 (推荐), macOS (需要编译)
- **Unreal Engine**: 5.4或更高版本
- **Visual Studio**: 2022 (Windows) 或 Xcode (macOS)
- **内存**: 至少16GB RAM
- **显卡**: 支持DirectX 11/12

### 安装步骤

1. **克隆仓库**
   ```bash
   git clone <repository-url>
   cd Crunch
   ```

2. **生成项目文件**
   ```bash
   # Windows
   ./GenerateProjectFiles.bat
   
   # 或者右键点击 Crunch.uproject -> Generate Visual Studio project files
   ```

3. **编译项目**
   ```bash
   # 使用Visual Studio编译，或者
   UnrealBuildTool.exe Crunch Win64 Development
   ```

4. **启动游戏**
   ```bash
   # 客户端
   ./launchScripts/launchGame.bat
   
   # 服务器
   ./launchScripts/launchServer.bat
   ```

## 🎯 核心系统详解

### 🧙‍♂️ 角色系统
- **角色定义**: 通过`UPDA_CharacterDefinition`数据资产定义角色属性
- **角色选择**: 支持多人同时选择角色，实时同步选择状态
- **角色展示**: 3D角色预览系统，支持动画播放

### ⚔️ 技能系统 (GAS)
- **技能升级**: 基于等级的技能点分配系统
- **技能冷却**: 实时冷却计时器和视觉反馈
- **技能效果**: 支持各种游戏效果和属性修改
- **技能UI**: 动态技能槽界面，显示图标、等级、冷却等
- **连锁攻击技能**: 新增连锁攻击技能(GA_ChainAttack)，可对范围内的多个目标进行连续攻击

### 🛍️ 商店与物品系统
- **物品合成**: 可视化合成树，展示物品依赖关系
- **动态定价**: 根据背包物品计算实际购买价格
- **库存管理**: 完整的物品存储和管理系统
- **物品提示**: 鼠标悬停显示详细物品信息
- **智能合成系统**: 改进的合成机制，支持自动查找背包中的合成材料

### 🌐 网络架构
- **会话管理**: 支持房间创建、加入和搜索
- **状态同步**: 玩家状态、角色选择实时同步
- **服务器部署**: Docker容器化部署，支持负载均衡
- **协调器集成**: 与Python协调器服务集成，支持会话管理和负载均衡

## 🔌 插件系统

### VRM4U插件
- **功能**: 支持VRM格式3D角色模型导入
- **用途**: 角色自定义和展示
- **版本**: 支持UE5.4

### KawaiiPhysics插件
- **功能**: 高级物理动画系统
- **用途**: 角色动画和物理效果

### RiderLink插件
- **功能**: JetBrains Rider IDE集成
- **用途**: 提供更好的C++开发体验

## 🐳 部署指南

### Docker部署

1. **构建服务器镜像**
   ```bash
   cd ServerDeploy/server
   docker build -t crunch-server .
   ```

2. **构建协调器镜像**
   ```bash
   cd ServerDeploy/coordinator
   docker build -t crunch-coordinator .
   ```

3. **启动服务**
   ```bash
   docker-compose up -d
   ```

### 手动部署

1. **打包游戏**
   - 在UE编辑器中: File -> Package Project -> Windows
   - 选择目标平台和配置

2. **配置服务器**
   - 复制打包后的文件到服务器
   - 配置防火墙端口
   - 运行服务器可执行文件

## 🎨 UI系统架构

### 主要UI组件

- **主菜单**: 游戏启动界面，会话管理
- **角色选择**: 多人角色选择界面
- **游戏HUD**: 技能栏、生命值、迷你地图等
- **商店界面**: 物品购买和合成界面
- **库存界面**: 物品管理和使用

### UI渲染系统
- **离屏渲染**: 3D模型在UI中的实时渲染
- **动态材质**: 支持参数化材质效果
- **响应式布局**: 适配不同分辨率

## 🔧 开发指南

### 代码规范

- **命名约定**: 
  - 类名使用`U`/`A`/`F`前缀
  - 变量使用驼峰命名法
  - 常量使用全大写+下划线

- **文件组织**:
  - 头文件放在`Public`文件夹
  - 实现文件放在`Private`文件夹
  - 按功能模块分类

### 调试技巧

1. **日志系统**
   ```cpp
   UE_LOG(LogTemp, Warning, TEXT("Debug message"));
   ```

2. **蓝图调试**
   - 使用断点和变量监视
   - Print String节点输出调试信息

3. **网络调试**
   - 使用`net.PackageMap.DebugObject`
   - 监控网络复制状态

## 📊 性能优化

### 渲染优化
- **LOD系统**: 多级细节模型
- **剔除优化**: 视锥体和遮挡剔除
- **材质优化**: 减少着色器复杂度

### 网络优化
- **数据压缩**: 减少网络传输量
- **预测系统**: 客户端预测减少延迟
- **批量更新**: 合并网络更新

## 🤝 贡献指南

### 提交代码

1. **创建分支**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **提交更改**
   ```bash
   git commit -m "Add: 新功能描述"
   ```

3. **推送分支**
   ```bash
   git push origin feature/your-feature-name
   ```

### 代码审查
- 确保代码符合项目规范
- 添加必要的注释和文档
- 通过所有单元测试

## 📝 更新日志

### v1.0.0 (开发中)
- ✅ 基础多人网络架构
- ✅ 角色选择系统
- ✅ 技能系统(GAS)集成
- ✅ 商店和物品系统
- ✅ 连锁攻击技能
- ✅ 弹幕聊天系统
- ✅ 智能合成系统
- ✅ 协调器集成
- 🔄 AI系统优化
- 🔄 UI界面完善

## 📚 完整文档

查看完整的项目文档：
- **[📖 文档索引](./docs/INDEX.md)** - 所有文档的导航和概览
- **[🏗️ 项目架构](./docs/ARCHITECTURE.md)** - 详细的系统架构说明
- **[🛠️ 开发指南](./docs/DEVELOPMENT.md)** - 开发环境和编码规范
- **[📚 API文档](./docs/API.md)** - 核心类和接口说明
- **[🚀 部署指南](./docs/DEPLOYMENT.md)** - 完整的部署方案

## 📞 联系方式

- **项目负责人**: 幻雨喜欢小猫咪
- **技术支持**: [GitHub Issues](./issues)
- **完整文档**: [项目文档库](./docs/)

## 📄 许可证

本项目遵循Epic Games许可协议。详情请参阅[LICENSE](./LICENSE)文件。

---

> 🎮 **Crunch** - 让每一场战斗都充满激情！