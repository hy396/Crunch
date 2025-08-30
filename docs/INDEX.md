# Crunch 项目文档索引

欢迎来到 **Crunch** 项目的完整文档库！这里包含了项目的所有技术文档和指南。

## 📚 文档概览

### 🚀 快速开始
- **[项目简介 (README.md)](../README.md)** - 项目概述、特性介绍和快速开始指南
- **[开发指南 (DEVELOPMENT.md)](./DEVELOPMENT.md)** - 详细的开发环境搭建和编码规范

### 🏗️ 架构与设计
- **[项目架构 (ARCHITECTURE.md)](./ARCHITECTURE.md)** - 完整的系统架构和设计模式说明
- **[API文档 (API.md)](./API.md)** - 核心类和接口的详细API说明

### 🚢 部署与运维
- **[部署指南 (DEPLOYMENT.md)](./DEPLOYMENT.md)** - 从开发到生产的完整部署方案

## 🎮 核心系统文档

### 角色系统
```
角色系统实现了完整的角色管理功能：
- 数据驱动的角色定义 (UPDA_CharacterDefinition)
- 角色选择和配置系统
- 3D角色预览功能
```

### 技能系统 (GAS)
```
基于Unreal Engine的Gameplay Ability System：
- 技能升级和管理
- 属性系统 (UCAttributeSet)
- 技能UI集成 (UAbilityGauge)
```

### 物品与商店系统
```
完整的经济系统实现：
- 物品合成树 (ITreeNodeInterface)
- 动态定价算法
- 库存管理 (UInventoryComponent)
```

### 网络架构
```
多人游戏网络解决方案：
- 会话管理和房间系统
- 状态同步和RPC通信
- 负载均衡和服务发现
```

## 📖 详细文档导航

### 🛠️ 开发者文档

| 文档 | 描述 | 适用人群 |
|------|------|----------|
| [开发环境搭建](./DEVELOPMENT.md#开发环境搭建) | IDE配置、插件安装、项目编译 | 新手开发者 |
| [代码规范](./DEVELOPMENT.md#代码规范) | 命名约定、文件组织、最佳实践 | 所有开发者 |
| [调试技巧](./DEVELOPMENT.md#调试技巧) | 日志使用、断点调试、性能分析 | 中级开发者 |

### 🏗️ 架构文档

| 模块 | 文档链接 | 核心类 |
|------|----------|--------|
| 游戏框架 | [Framework模块](./ARCHITECTURE.md#游戏框架模块) | `UMGameInstance`, `ACrunchPlayerController` |
| 角色系统 | [Character模块](./ARCHITECTURE.md#角色系统) | `ACrunchCharacter`, `UPDA_CharacterDefinition` |
| 技能系统 | [GAS集成](./ARCHITECTURE.md#技能系统) | `UCAttributeSet`, `UGameplayAbility_Base` |
| UI系统 | [UI架构](./ARCHITECTURE.md#ui系统架构) | `UShopWidget`, `UAbilityGauge` |
| 网络系统 | [网络架构](./ARCHITECTURE.md#网络架构) | `FPlayerSelection`, RPC函数 |

### 📚 API参考

| 系统 | 主要类 | API文档链接 |
|------|--------|-------------|
| 游戏实例 | `UMGameInstance` | [游戏框架API](./API.md#核心游戏框架) |
| 角色定义 | `UPDA_CharacterDefinition` | [角色系统API](./API.md#角色系统) |
| 属性系统 | `UCAttributeSet` | [技能系统API](./API.md#技能系统-gas) |
| 库存管理 | `UInventoryComponent` | [库存系统API](./API.md#库存系统) |
| UI控件 | `UShopWidget`, `UAbilityGauge` | [UI系统API](./API.md#ui系统) |

### 🚀 部署指南

| 部署方式 | 文档链接 | 适用场景 |
|----------|----------|----------|
| Docker部署 | [Docker指南](./DEPLOYMENT.md#docker-部署推荐) | 开发/测试/生产 |
| 传统服务器 | [服务器部署](./DEPLOYMENT.md#传统服务器部署) | 本地服务器 |
| 云平台部署 | [云部署](./DEPLOYMENT.md#云平台部署) | 生产环境 |

## 🔍 快速索引

### 常用代码示例

#### 创建新角色
```
// 参考：DEVELOPMENT.md#角色系统开发
UCLASS()
class CRUNCH_API UGA_NewAbility : public UGameplayAbility
{
    // 实现细节...
};
```

#### 添加UI控件
```
// 参考：DEVELOPMENT.md#ui系统开发
UCLASS()
class CRUNCH_API UNewWidget : public UUserWidget
{
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ActionButton;
};
```

#### 网络函数实现
```
// 参考：DEVELOPMENT.md#网络开发
UFUNCTION(Server, Reliable)
void Server_TakeDamage(int32 Damage);
```

### 配置文件位置

| 配置类型 | 文件路径 | 说明 |
|----------|----------|------|
| 游戏设置 | `Config/DefaultGame.ini` | 游戏基础配置 |
| 引擎设置 | `Config/DefaultEngine.ini` | 引擎和渲染配置 |
| 输入映射 | `Config/DefaultInput.ini` | 输入绑定配置 |
| Docker配置 | `docker-compose.yml` | 容器部署配置 |

### 重要目录结构

```
Crunch/
├── 📁 Source/Crunch/          # 核心C++代码
│   ├── 📁 Private/           # 实现文件
│   │   ├── 📁 Character/     # 角色系统
│   │   ├── 📁 GAS/          # 技能系统
│   │   ├── 📁 Inventory/    # 库存系统
│   │   ├── 📁 UI/           # 用户界面
│   │   └── 📁 Network/      # 网络系统
│   └── 📁 Public/           # 头文件
├── 📁 Config/               # 配置文件
├── 📁 Plugins/              # 第三方插件
├── 📁 ServerDeploy/         # 部署文件
└── 📁 docs/                # 项目文档
```

## 🎯 学习路径推荐

### 新手开发者
1. 📖 阅读 [项目简介](../README.md)
2. 🛠️ 按照 [开发环境搭建](./DEVELOPMENT.md#开发环境搭建) 配置环境
3. 🎮 了解 [核心系统](./ARCHITECTURE.md#核心模块详解)
4. 💻 练习 [代码示例](./DEVELOPMENT.md#核心系统开发)

### 有经验的开发者
1. 🏗️ 深入理解 [项目架构](./ARCHITECTURE.md)
2. 📚 查阅 [API文档](./API.md) 了解接口细节
3. 🔧 参考 [最佳实践](./DEVELOPMENT.md#最佳实践)
4. 🚀 学习 [部署方案](./DEPLOYMENT.md)

### 运维工程师
1. 🐳 掌握 [Docker部署](./DEPLOYMENT.md#docker-部署推荐)
2. ☁️ 了解 [云平台部署](./DEPLOYMENT.md#云平台部署)
3. 📊 配置 [监控系统](./DEPLOYMENT.md#监控与日志)
4. 🔧 学习 [故障排除](./DEPLOYMENT.md#故障排除)

## 🤝 贡献指南

### 文档贡献
- 发现错误或改进建议？请提交 [Issue](../issues)
- 想要完善文档？欢迎提交 [Pull Request](../pulls)
- 文档使用 Markdown 格式，请保持统一的格式风格

### 代码贡献
1. 遵循 [代码规范](./DEVELOPMENT.md#代码规范)
2. 编写相应的单元测试
3. 更新相关文档
4. 通过代码审查

## 📞 获取帮助

### 问题反馈
- **Bug报告**: [GitHub Issues](../issues)
- **功能请求**: [Feature Requests](../issues/new?template=feature_request.md)
- **文档问题**: [Documentation Issues](../issues/new?template=documentation.md)

### 技术支持
- **开发问题**: 查阅 [开发指南](./DEVELOPMENT.md) 或在Issues中提问
- **部署问题**: 参考 [部署指南](./DEPLOYMENT.md) 或联系运维团队
- **架构问题**: 阅读 [架构文档](./ARCHITECTURE.md) 了解设计理念

## 📄 许可证

本项目遵循 Epic Games 许可协议。详情请参阅 [LICENSE](../LICENSE) 文件。

---

> 🎮 **Crunch** - 让每一场战斗都充满激情！
> 
> 📚 文档持续更新中，欢迎贡献和反馈！