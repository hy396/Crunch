# Crunch 项目架构文档

## 🏗️ 整体架构概览

Crunch是一个基于Unreal Engine 5.4开发的多人对战游戏，采用了模块化的架构设计，确保代码的可维护性和扩展性。

### 架构设计原则
- **模块化**: 各系统独立封装，降低耦合
- **数据驱动**: 使用数据资产和配置驱动游戏内容
- **事件驱动**: 基于委托和事件系统实现松耦合通信
- **组件化**: Actor功能通过组件组合实现

## 📦 核心模块框架

### 1. 游戏框架模块 (Framework)

#### 核心组件
- **游戏模式管理器**: 控制不同游戏阶段的流程
- **玩家状态管理**: 追踪玩家的游戏数据和状态
- **游戏会话**: 管理多人游戏的房间和匹配

**关键类**
```cpp
// 游戏实例 - 管理整个游戏生命周期
class UMGameInstance : public UGameInstance

// 游戏模式 - 定义游戏规则和流程
class ACrunchGameMode : public AGameModeBase

// 玩家控制器 - 处理玩家输入和网络通信
class ACPlayerController : public APlayerController
```

**详细文档**: 见开发指南

### 2. 角色系统 (Character)

**设计理念**
- 数据驱动的角色定义
- 模块化的角色组件
- 支持运行时角色切换

**核心类**
```cpp
// 角色定义数据资产
class UPDA_CharacterDefinition : public UPrimaryDataAsset

// 角色基类
class ACCharacter : public ACharacter

// 角色组件系统
class UCharacterComponent : public UActorComponent
```

**详细文档**: 见角色系统部分

### 3. 技能系统 (GAS Integration)

**技术栈**
- Gameplay Ability System (GAS)
- 属性系统 (Attribute Set)
- 游戏效果 (Gameplay Effect)

**核心组件**
```cpp
// 属性集 - 定义角色的各种属性
class UCAttributeSet : public UAttributeSet

// 技能系统组件
class UAbilitySystemComponent : public UAbilitySystemComponent

// 游戏效果 - 技能的实际效果实现
class UGameplayEffect : public UGameplayEffect
```

**详细文档**: [GAS系统文档](./GAS_SYSTEM.md)

### 4. 物品与库存系统 (Inventory)

**设计模式**
- 组合模式: 物品合成树结构
- 策略模式: 不同物品类型的行为
- 观察者模式: 库存变化通知UI

**核心类**
```cpp
// 商店物品数据资产
class UPDA_ShopItem : public UPrimaryDataAsset

// 库存组件
class UInventoryComponent : public UActorComponent

// 物品树节点接口
class ITreeNodeInterface
```

**详细文档**: 见API文档

### 5. 前端UI系统 (FrontendUI)

**核心职责**
- 前端菜单和设置界面
- 选项设置系统
- Widget基类和管理

**关键类**
```cpp
// 前端UI子系统
class UFrontendUISubsystem : public UGameInstanceSubsystem

// 可激活Widget基类
class UWidget_ActivatableBase : public UUserWidget

// 主布局容器
class UWidget_PrimaryLayout : public UUserWidget

// 选项数据注册表
class UOptionsDataRegistry : public UDataRegistry
```

**详细文档**: [前端UI系统文档](./FRONTEND_UI_SYSTEM.md)

### 6. 聊天系统 (Chat System)

**核心职责**
- 实时多人聊天功能
- 弹幕模式支持
- 多频道和发送者识别

**关键类**
```cpp
// 聊天界面主控件
class UChatWidget : public UUserWidget

// 消息项显示控件
class UChatMessageItemWidget : public UUserWidget

// 聊天接口
class IChatInterface
```

**详细文档**: [聊天系统文档](./CHAT_SYSTEM.md)

### 7. AI系统 (Artificial Intelligence)

**技术栈**
- 行为树 (Behavior Tree)
- 感知系统 (AI Perception)
- 导航网格 (NavMesh)

**核心类**
```cpp
// AI控制器基类
class ACrunchAIController : public AAIController

// 行为树组件
class UBehaviorTreeComponent : public UBehaviorTreeComponent

// 黑板组件
class UBlackboardComponent : public UBlackboardComponent
```

**详细文档**: 见开发指南

## 🌐 网络架构 (Networking)

### 网络拓扑

```
网络架构图:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Client A  │    │   Client B  │    │   Client C  │
└──────┬──────┘    └──────┬──────┘    └──────┬──────┘
       │                  │                  │
       └─────────┬────────┴────────┬─────────┘
                 │                 │
         ┌───────▼─────────┐ ┌─────▼──────┐
         │  Game Server    │ │ Coordinator │
         │  (UE5 Process)  │ │  (Python)   │
         └─────────────────┘ └────────────┘
```

### 协调器功能
- **会话管理**: 创建、查找和管理游戏会话
- **负载均衡**: 分配玩家到合适的服务器
- **状态同步**: 同步会话状态和玩家信息

### 同步策略
- **状态复制**: 玩家位置、生命值等关键数据
- **事件复制**: 技能释放、物品使用等动作
- **可靠传输**: 重要游戏事件确保送达
- **不可靠传输**: 位置更新等高频数据

### RPC通信优化
- 优化的RPC调用机制
- 确保网络通信的实时性和可靠性
- 增强的错误处理和重连机制

**详细文档**: [部署指南](./DEPLOYMENT.md)

## 🔧 关键设计模式

### 1. 组件化架构

```cpp
// 示例：角色组件化设计
class ACrunchCharacter : public ACharacter
{
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UInventoryComponent> InventoryComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCharacterStatsComponent> StatsComponent;
};
```

### 2. 数据驱动设计

```cpp
// 角色定义使用数据资产
UCLASS(BlueprintType)
class UPDA_CharacterDefinition : public UPrimaryDataAsset
{
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<USkeletalMesh> CharacterMesh;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UAnimInstance> AnimBlueprint;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;
};
```

### 3. 事件驱动通信

```cpp
// 使用委托实现松耦合通信
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float /*NewHealth*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemPurchased, const UPDA_ShopItem*, int32 /*Quantity*/);
```

### 4. 注册表模式 (FrontendUI)

选项数据通过注册表统一管理，支持条件编辑和动态数据类型

### 5. 模板方法模式 (Widget系统)

基类定义通用逻辑，子类实现特定功能

## 🔗 模块依赖关系

```
graph TB
    Framework[Framework] --> Character[Character]
    Framework --> Network[Network]
    Framework --> FrontendUI[FrontendUI]
    
    Character --> GAS[GAS System]
    Character --> AI[AI System]
    Character --> Inventory[Inventory]
    
    FrontendUI --> Inventory
    FrontendUI --> GAS
    FrontendUI --> Chat[Chat System]
    
    Chat --> Network
    Chat --> Framework
    
    Inventory --> GAS
    AI --> GAS
    
    Network --> Framework
    
    GameplayUI[Gameplay UI] --> Inventory
    GameplayUI --> GAS
    GameplayUI --> Chat
```

## 📊 性能考虑

### 1. 内存管理
- **对象池**: 频繁创建销毁的对象使用对象池
- **智能指针**: 使用TObjectPtr管理UObject引用
- **垃圾回收**: 合理安排UObject的生命周期

### 2. 渲染优化
- **LOD系统**: 根据距离调整模型细节
- **剔除系统**: 视锥体剔除和遮挡剔除
- **批次优化**: 减少Draw Call数量

### 3. 网络优化
- **数据压缩**: 使用UE5内置的网络压缩
- **更新频率**: 根据重要性调整复制频率
- **预测系统**: 客户端预测减少延迟感知

## 🚀 扩展性设计

### 1. 插件系统
- 支持第三方插件集成
- 模块化的功能扩展
- 热插拔能力

### 2. 配置系统
- 游戏平衡性参数可配置
- 运行时参数调整
- 数据表驱动的内容

### 3. 多平台支持
- 跨平台网络兼容
- 平台特定的优化
- 移动端适配

## 📈 监控与调试

### 1. 性能监控
- **Stat命令**: 实时性能数据
- **Profiler**: 详细性能分析
- **网络调试**: 网络延迟和丢包监控

### 2. 日志系统
```cpp
// 自定义日志类别
DECLARE_LOG_CATEGORY_EXTERN(LogCrunchGame, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCrunchNetwork, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCrunchUI, Log, All);
```

### 3. 调试工具
- **蓝图调试器**: 可视化蓝图执行
- **世界大纲视图**: 场景对象层次
- **网络复制视图**: 网络数据监控

## 📚 相关文档

- **[📖 文档索引](./INDEX.md)** - 所有文档的导航和概览
- **[🎮 前端UI系统](./FRONTEND_UI_SYSTEM.md)** - 前端UI完整框架
- **[⚔️ GAS系统](./GAS_SYSTEM.md)** - GAS技能系统详解
- **[💬 聊天系统](./CHAT_SYSTEM.md)** - 聊天系统详解
- **[🛠️ 开发指南](./DEVELOPMENT.md)** - 开发环境和编码规范
- **[📚 API文档](./API.md)** - 核心类和接口说明
- **[🚀 部署指南](./DEPLOYMENT.md)** - 完整的部署方案

这个架构文档提供了Crunch项目的整体技术架构视图，各子系统的详细实现请参考对应的专门文档。
