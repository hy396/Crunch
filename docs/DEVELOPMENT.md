# 开发指南

## 环境搭建

### 必需软件

- **Unreal Engine 5.4** — 通过 Epic Games Launcher 安装
- **Visual Studio 2022+** — 安装"使用 C++ 的游戏开发"工作负载
- **Git** — 版本控制

### 推荐工具

- **JetBrains Rider** — 项目已集成 RiderLink 插件
- **Visual Studio Code** — 轻量编辑，配合 clangd 扩展

### 首次构建

1. 克隆仓库
2. 右键 `Crunch.uproject` → "Generate Visual Studio project files"
3. 打开 `Crunch.sln`，选择 `CrunchEditor` 为启动项目
4. 构建并运行（F5）

命令行构建：

```bash
# 编辑器
Engine/Build/BatchFiles/Build.bat CrunchEditor Win64 Development -project="<路径>/Crunch.uproject"

# 专用服务器
Engine/Build/BatchFiles/Build.bat CrunchServer Win64 Development -project="<路径>/Crunch.uproject"
```

### VRM4U 插件

VRM4U 插件的 assimp 库（`Plugins/VRM4U/ThirdParty/assimp/lib/x64/Release/assimp-vc141-mt.lib`）因文件体积未纳入 Git。首次编译前需要：

1. 从 [ruyo/assimp](https://github.com/ruyo/assimp) 克隆源码
2. 用 CMake + MSVC 编译 Release 共享库
3. 将 `.lib` 放入 `Plugins/VRM4U/ThirdParty/assimp/lib/x64/Release/`
4. 将 `.dll` 放入 `Plugins/VRM4U/ThirdParty/assimp/bin/x64/`

注意：编译产物文件名可能是 `assimp-vc14x-mt`，需重命名为 `assimp-vc141-mt`。

## 编码规范

### 命名约定

遵循 UE5 标准：

- 类前缀：`U`（UObject）、`A`（Actor）、`F`（结构体/值类型）、`E`（枚举）、`I`（接口）
- 布尔变量：`b` 前缀（`bIsDead`、`bIsActive`）
- 委托：`FOn...Delegate`（`FOnItemAddedDelegate`）
- GameplayTag：`Ability.SkillName.EventName` 层级结构
- 数据资产：`PDA_` 前缀（`PDA_CharacterDefinition`、`PDA_ShopItem`）

### 文件组织

项目采用 Private-only 布局（头文件和实现文件都在 `Private/` 下），按功能模块分目录：

```text
Private/
├── ModuleName/
│   ├── ClassName.h
│   └── ClassName.cpp
```

新增文件时遵循现有目录划分：

- 技能类（`GA_*`）→ `GAS/Abilities/`
- Actor 类（投射物、陷阱等）→ `Actor/`
- UI Widget → `UI/` 对应子目录
- 框架类 → `Framework/`

### 头文件 Include 规则

- 自身头文件在第一行：`#include "ClassName.h"`
- 使用从 `Private/` 开始的相对路径：`#include "GAS/Core/CGameplayAbility.h"`
- 同目录文件可直接引用：`#include "ClassName.h"`

### 网络代码规范

Server RPC 必须带 `WithValidation`：

```cpp
UFUNCTION(Server, Reliable, WithValidation)
void Server_DoSomething(/* params */);
```

属性同步选择合适的条件：

- `COND_None` — 所有客户端都需要（血量等战斗属性）
- `COND_OwnerOnly` — 只有属主需要（金币、经验）
- `COND_InitialOnly` — 初始化后不变的值

Meta 属性（服务器用完即清零的中间值）不要加同步标记。

## 本地测试

### 单机多人

在编辑器中：Play → Advanced Settings → Number of Players = 2+，Net Mode = Play As Listen Server。

### 独立进程

```bash
# 终端1：启动服务器
launchScripts/launchServer.bat

# 终端2：启动客户端
launchScripts/launchGame.bat
```

### 测试特定系统

- **技能测试**：在编辑器中放置角色，赋予技能蓝图，PIE 测试
- **网络测试**：使用 2 个客户端验证同步，观察 `Net.PackageMap.DebugObject` 日志
- **UI 测试**：Widget Reflector（`Ctrl+Shift+W`）检查层级

## 调试

### 日志

项目自定义日志类别：

```cpp
// GAS 系统
DECLARE_LOG_CATEGORY_EXTERN(LogCrunchGAS, Log, All);

// 使用便捷宏
GAS_LOG(TEXT("Message %s"), *Value);
GAS_WARN(TEXT("Warning %s"), *Value);
GAS_LOG_ASC(ASC, TEXT("ASC state: %s"), *Info);
```

### GAS 调试

```cpp
// 打印完整 ASC 状态
UGASDebugHelper::PrintASCState(ASC, true);

// 注册监控（技能激活/失败/结束自动打日志）
UGASDebugHelper::RegisterPredictionFailureMonitor(ASC);
```

控制台命令：

```text
showdebug abilitysystem          显示 GAS 调试 HUD
AbilitySystem.Debug.NextTarget   切换调试目标
```

### 网络调试

```text
net.PackageMap.DebugObject       网络对象追踪
stat net                         网络统计
p.NetShowCorrections 1           显示位置校正
```

### 性能分析

```text
stat fps                         帧率
stat unit                        帧耗时分解
stat game                        游戏线程
stat gpu                         GPU 耗时
profilegpu                       GPU 详细分析
```

## 添加新内容的常见流程

### 添加新英雄技能

1. 在 `GAS/Abilities/` 创建 `GA_NewSkill.h/.cpp`，继承 `UCGameplayAbility`
2. 在 `TGameplayTags.h/.cpp` 添加标签
3. 如需 Actor（投射物/陷阱），在 `Actor/` 创建
4. 创建蓝图（GE、技能蓝图、蒙太奇）
5. 添加到角色的 `PDA_CharacterDefinition`

详见 [GAS_Mage_Abilities.md](GAS_Mage_Abilities.md)。

### 添加新商店物品

1. 创建 `UPDA_ShopItem` 数据资产
2. 配置属性：名称、价格、图标、效果
3. 如需主动技能，创建 `UGameplayAbility` 子类
4. 如有合成关系，在 `IngredientItems` 中设置材料引用

### 添加新 UI Widget

游戏内 HUD：
1. 创建 `UUserWidget` C++ 类（如需逻辑）
2. 创建蓝图 Widget，绑定组件
3. 在 `ACPlayerController` 中添加创建和显示逻辑

前端界面：
1. 创建继承 `UWidget_ActivatableBase` 的类
2. 在 `UFrontendDeveloperSettings::FrontendWidgetMap` 注册 Tag → Class 映射
3. 通过 `UFrontendUISubsystem::PushSoftWidgetToStackAsync()` 推入
