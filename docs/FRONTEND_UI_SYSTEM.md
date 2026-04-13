# Frontend UI 系统

## 概述

Crunch 的前端 UI（主菜单、选项设置、键位绑定等）基于 UE5 的 CommonUI 插件构建，采用 Widget Stack 架构管理界面层级。与游戏内 HUD（传统 UMG）是两套独立系统。

## 架构

```text
UFrontendUISubsystem (GameInstance 子系统, 全局单例)
  │
  ├── UWidget_PrimaryLayout (主布局, CommonUserWidget)
  │     │
  │     └── TMap<FGameplayTag, UCommonActivatableWidgetContainerBase>
  │           ├── WidgetStack::Modal      模态弹窗层（最高优先级）
  │           ├── WidgetStack::GameMenu   游戏菜单层
  │           ├── WidgetStack::GameHud    游戏 HUD 层
  │           └── WidgetStack::Frontend   前端界面层
  │
  └── Widget 推入/弹出通过 Tag 定位到对应 Stack
```

### 核心流程

1. `UWidget_PrimaryLayout` 蓝图在关卡中创建后，调用 `RegisterCreatedPrimaryLayoutWidget()` 注册到子系统
2. 布局蓝图中的每个 `UCommonActivatableWidgetContainerBase` 通过 `RegisterWidgetStack(Tag, Stack)` 注册
3. 需要显示界面时，调用 `PushSoftWidgetToStackAsync(StackTag, SoftWidgetClass, Callback)` 异步加载并推入
4. Widget 基于 `UCommonActivatableWidget` 的激活/反激活机制自动管理输入焦点

## 核心类

### UFrontendUISubsystem

`UGameInstanceSubsystem`，全局访问：

```cpp
UFrontendUISubsystem* Subsystem = UFrontendUISubsystem::Get(WorldContextObject);
```

关键方法：

| 方法 | 说明 |
| ---- | ---- |
| `RegisterCreatedPrimaryLayoutWidget(Widget)` | 注册主布局 |
| `PushSoftWidgetToStackAsync(StackTag, SoftClass, Callback)` | 异步推入 Widget |
| `PushConfirmScreenToModalStackAsync(Type, Title, Message, Callback)` | 推入确认弹窗 |
| `SetPrimaryLayoutVisibility(bVisible)` | 控制主布局可见性 |
| `IsPrimaryLayoutRegistered()` | 检查布局是否已注册 |

`EAsyncPushWidgetState` 回调状态：
- `OnCreatedBeforePush` — Widget 创建完成，尚未推入（可在此配置）
- `AfterPush` — 已推入 Stack

### UWidget_ActivatableBase

所有前端 Widget 的基类，继承 `UCommonActivatableWidget`：

- `GetOwningFrontendPlayerController()` — 获取所属 PlayerController（带缓存）
- 子类在蓝图中重写 `NativeOnActivated` / `NativeOnDeactivated` 处理激活逻辑

### UWidget_PrimaryLayout

主布局容器，继承 `UCommonUserWidget`：

- `FindWidgetStackByTag(Tag)` — 通过 GameplayTag 查找 Widget Stack
- `RegisterWidgetStack(Tag, Stack)` — 注册 Stack（蓝图中在 Construct 时调用）
- 内部维护 `TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*>`

## GameplayTags

在 `FrontendGameplayTags` 命名空间中定义：

**Widget Stack 标签**（定位目标 Stack）：

```text
FrontendGameplayTags::WidgetStack::Modal
FrontendGameplayTags::WidgetStack::GameMenu
FrontendGameplayTags::WidgetStack::GameHud
FrontendGameplayTags::WidgetStack::Frontend
```

**Widget 标签**（标识具体界面）：

```text
Widget::PressAnyKeyScreen    按任意键界面
Widget::MainMenuScreen       主菜单
Widget::ConfirmScreen        确认弹窗
Widget::OptionsScreen        选项界面
Widget::KeyRemapScreen       键位绑定
Widget::CreditScreen         制作人员
Widget::GameplayHUD          游戏 HUD
```

**音频标签**（选项系统用）：

```text
Audio::OverallVolume         总音量
Audio::MusicVolume           音乐音量
Audio::SoundFXVolume         音效音量
Audio::AllowBackgroundAudio  后台播放
Audio::UseHDRAudioMode       HDR 音频
```

## 选项系统

### 数据架构

```text
UOptionsDataRegistry (注册表, 管理所有选项页签)
  │
  ├── UListDataObject_Collection  "Gameplay" 页签
  │     ├── UListDataObject_String      (游戏难度)
  │     └── ...
  │
  ├── UListDataObject_Collection  "Audio" 页签
  │     ├── UListDataObject_Scalar      (总音量)
  │     ├── UListDataObject_Scalar      (音乐音量)
  │     ├── UListDataObject_Scalar      (音效音量)
  │     └── ...
  │
  ├── UListDataObject_Collection  "Video" 页签
  │     ├── UListDataObject_StringResolution  (分辨率)
  │     ├── UListDataObject_String           (窗口模式)
  │     ├── UListDataObject_Scalar           (亮度)
  │     └── ...
  │
  └── UListDataObject_Collection  "Controls" 页签
        └── UListDataObject_KeyRemap  (每个可绑定操作)
```

### UListDataObject_Base

选项数据项基类，核心功能：

- `DataID` — 唯一标识符
- `DataDisplayName` — 显示名称
- `DescriptionRichText` — 富文本说明
- `SoftDescriptionImage` — 说明图片（软引用）
- `HasDefaultValue()` / `TryResetBackToDefaultValue()` — 默认值管理
- `AddEditCondition()` / `IsDataCurrentlyEditable()` — 编辑条件（灰度控制）
- `AddEditDependencyData()` — 依赖项联动
- `OnListDataModified` 委托 — 数据变更通知
- `SetShouldApplySettingsImmediately(bool)` — 是否立即应用（vs 延迟到确认）

子类：

| 类 | 用途 | 对应 Widget |
| ---- | ---- | ---- |
| `UListDataObject_Scalar` | 数值滑块 | `Widget_ListEntry_Scalar` |
| `UListDataObject_String` | 字符串选择 | `Widget_ListEntry_String` |
| `UListDataObject_StringResolution` | 分辨率选择 | `Widget_ListEntry_String` |
| `UListDataObject_KeyRemap` | 键位绑定 | `Widget_ListEntry_KeyRemap` |
| `UListDataObject_Value` | 只读数值 | — |
| `UListDataObject_Collection` | 页签容器 | — |

### UOptionsDataRegistry

选项数据注册表，在 `Widget_OptionsScreen` 首次打开时初始化：

- `InitOptionsDataRegistry(LocalPlayer)` — 初始化所有页签
- `InitGameplayCollectionTab()` — 游戏选项
- `InitAudioCollectionTab()` — 音频选项
- `InitVideoCollectionTab()` — 视频选项
- `InitControlCollectionTab(LocalPlayer)` — 操作选项（读取当前键位绑定）
- `GetListSourceItemsBySelectedTabID(TabID)` — 获取指定页签的扁平化数据列表

### Widget_OptionsScreen

选项界面，绑定组件：

- `TabListWidget_OptionsTabs` — 页签切换
- `CommonListView_OptionsList` — 选项列表
- `DetailView_ListEntryInfo` — 右侧详情面板

交互流程：
1. 切换页签 → `OnOptionsTabSelected()` → 更新列表数据源
2. 选中/悬停条目 → 更新详情面板（说明文本+图片）
3. 修改数据 → `OnListViewListDataModified()` → 标记为可重置
4. 按重置键 → `OnResetBoundActionTriggered()` → 当前页签所有数据回到默认值

## 设置持久化

### UFrontendDeveloperSettings

`UDeveloperSettings`（项目级配置，编辑器中设置）：

- `FrontendWidgetMap` — GameplayTag → Widget 类映射
- `OptionsScreenSoftImageMap` — GameplayTag → 说明图片映射
- `MasterBusMix` — 主音频混合器引用
- `VolumeBusMap` — GameplayTag → SoundClass 映射

### UFrontendGameUserSettings

`UGameUserSettings`（用户级配置，保存到本地）：

- 游戏设置：`CurrentGameDifficulty`
- 音频设置：`OverallVolume`, `MusicVolume`, `SoundFXVolume`, `bAllowBackgroundAudio`, `bUseHDRAudioMode`
- 视频设置：`CurrentDisplayGamma`
- `ApplySettings()` — 应用所有设置
- `ApplyAudioSettings()` — 应用音频设置到 SoundMix 系统

## 通用组件

### UFrontendCommonButtonBase

按钮基类（继承 `UCommonButtonBase`）：

- `SetButtonText(Text)` / `GetButtonDisplayText()` — 按钮文本
- `SetButtonDisplayImage(Brush)` — 按钮图片
- `ButtonDescriptionText` — 悬停时通过 Subsystem 广播描述文本
- `bUserUpperCaseForButtonText` — 自动大写

### 异步操作

`UAsyncAction_PushConfirmScreen` — 蓝图可用的异步确认弹窗：

```cpp
// 蓝图中使用
auto* Action = UAsyncAction_PushConfirmScreen::PushConfirmScreen(
    WorldContext, ScreenType, Title, Message);
Action->OnButtonClicked.AddDynamic(this, &HandleResult);
```

`EConfirmScreenType` 定义弹窗样式，`EConfirmScreenButtonType` 返回用户点击的按钮。
