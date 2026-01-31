# Frontend UI 系统文档

## 📖 目录

- [系统概述](#系统概述)
- [核心架构](#核心架构)
- [子系统详解](#子系统详解)
- [Widget系统](#widget系统)
- [选项设置系统](#选项设置系统)
- [设置管理](#设置管理)
- [异步操作](#异步操作)
- [设计模式](#设计模式)
- [最佳实践](#最佳实践)

---

## 系统概述

Frontend UI 系统是一个基于 UE5 CommonUI 构建的现代化前端界面框架，提供了一套完整的 UI 组件和架构来管理游戏的前端界面。

### 设计理念

- **模块化设计**: 每个功能模块独立封装，职责清晰
- **数据驱动**: 使用 GameUserSettings 进行配置持久化
- **异步加载**: 使用软引用和异步操作优化资源加载
- **GameplayTag 驱动**: 使用 GameplayTag 进行配置和映射
- **CommonUI 集成**: 基于 UE5 CommonUI 实现标准化界面

### 系统特点

- ✅ 支持动态Widget加载和缓存
- ✅ 完整的选项设置系统（视频、音频、控制、游戏性）
- ✅ 键位重映射功能
- ✅ 模态对话框系统
- ✅ 响应式UI设计
- ✅ 多语言支持
- ✅ 手柄/键盘导航支持

---

## 核心架构

### 架构层次

```
FrontendUI/
├── Subsystems/           # 子系统层（核心管理）
│   └── FrontendUISubsystem
├── Widgets/              # Widget层（UI组件）
│   ├── Widget_ActivatableBase     # 可激活Widget基类
│   ├── Widget_PrimaryLayout        # 主布局Widget
│   ├── Widget_ConfirmScreen       # 确认对话框
│   ├── Components/                # 通用组件
│   │   ├── FrontendCommonButtonBase
│   │   ├── FrontendCommonListView
│   │   ├── FrontendCommonRotator
│   │   └── FrontendTabListWidgetBase
│   └── Options/                   # 选项界面
│       ├── Widget_OptionsScreen
│       ├── Widget_OptionsDetailsView
│       ├── Widget_KeyRemapScreen
│       ├── DataObjects/            # 选项数据对象
│       │   ├── ListDataObject_Base
│       │   ├── ListDataObject_Collection
│       │   ├── ListDataObject_Value
│       │   ├── ListDataObject_Scalar
│       │   ├── ListDataObject_String
│       │   ├── ListDataObject_KeyRemap
│       │   └── ListDataObject_StringResolution
│       └── ListEntries/           # 列表条目Widget
│           ├── Widget_ListEntry_Base
│           ├── Widget_ListEntry_KeyRemap
│           ├── Widget_ListEntry_Scalar
│           └── Widget_ListEntry_String
├── FrontendSettings/      # 设置管理层
│   ├── FrontendDeveloperSettings    # 开发者配置
│   └── FrontendGameUserSettings     # 用户设置持久化
├── FrontendTypes/          # 类型定义层
│   ├── FrontendEnumTypes             # 枚举类型
│   └── FrontendStructTypes           # 结构体类型
├── AsyncActions/          # 异步操作层
│   ├── AsyncAction_PushSoftWidget
│   └── AsyncAction_PushConfirmScreen
├── Core/                  # 核心层
│   └── FrontendGameplayTags
└── FrontendFunctionLibrary.h  # 功能库
```

---

## 子系统详解

### 1. UFrontendUISubsystem

**职责**: FrontendUI 的核心子系统，管理 UI 生命周期和 Widget 栈

**核心功能**:

- 主布局Widget注册和管理
- 异步Widget加载和推送
- 模态对话框管理
- UI栈操作

**关键属性**:
```cpp
// 创建的主布局Widget（Transient标记，不序列化）
UPROPERTY(Transient)
UWidget_PrimaryLayout* CreatedPrimaryLayout;

// 按钮描述文本更新委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, 
    UFrontendCommonButtonBase*, BroadcastingButton, FText, DescriptionText);
UPROPERTY(BlueprintAssignable)
FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;
```

**关键方法**:
```cpp
// 获取子系统实例
static UFrontendUISubsystem* Get(const UObject* WorldContextObject);

// 注册主布局Widget
void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget);

// 异步推送Widget到指定栈
void PushSoftWidgetToStackAsync(
    const FGameplayTag& InWidgetStackTag,
    TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
    TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AysncPushStateCallback
);

// 异步推送确认对话框
void PushConfirmScreenToModalStackAsync(
    EConfirmScreenType InScreenType,
    const FText& InScreenTitle,
    const FText& InScreenMessage,
    TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback
);

// 设置主布局可见性
bool SetPrimaryLayoutVisibility(bool bVisible);

// 检查主布局是否已注册
bool IsPrimaryLayoutRegistered() const;
```

**使用场景**:
1. 初始化主布局时注册
2. 动态加载并显示界面
3. 显示确认对话框
4. 控制主布局显示状态

---

## Widget系统

### 1. UWidget_ActivatableBase

**职责**: 所有可激活Widget的基类，提供统一的激活/停用机制

**继承关系**:
```
UCommonActivatableWidget (CommonUI)
    ↓
UWidget_ActivatableBase
    ↓
    ├── Widget_PrimaryLayout
    ├── Widget_ConfirmScreen
    ├── Widget_OptionsScreen
    └── ... 其他UI Widget
```

**关键特性**:
- 禁用默认Tick优化性能 (`meta = (DisableNaiveTick)`)
- 自动缓存所属PlayerController
- 支持CommonUI的激活栈机制

**关键方法**:
```cpp
// 获取所属的PlayerController
UFUNCTION(BlueprintPure)
APlayerController* GetOwningFrontendPlayerController();
```

**使用示例**:
```cpp
// 在子类中使用
class UMyCustomWidget : public UWidget_ActivatableBase
{
protected:
    virtual void NativeOnActivated() override
    {
        Super::NativeOnActivated();
        // 界面激活时的逻辑
    }
    
    virtual void NativeOnDeactivated() override
    {
        Super::NativeOnDeactivated();
        // 界面停用时的逻辑
    }
};
```

---

### 2. UWidget_PrimaryLayout

**职责**: 前端界面的主布局容器，管理多个Widget栈

**核心功能**:
- 管理多个UI栈（如主菜单栈、模态栈等）
- 通过GameplayTag注册和查找Widget栈
- 为所有子界面提供容器

**关键方法**:
```cpp
// 根据GameplayTag查找Widget栈
UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InTag) const;

// 注册Widget栈
UFUNCTION(BlueprintCallable)
void RegisterWidgetStack(
    UPARAM(meta=(Categories="Frontend.WidgetStack")) FGameplayTag InStackTag,
    UCommonActivatableWidgetContainerBase* InStack
);
```

**使用流程**:
1. 创建PrimaryLayout Widget实例
2. 在子系统中注册 (`FrontendUISubsystem::RegisterCreatedPrimaryLayoutWidget`)
3. 注册多个Widget栈（主栈、模态栈等）
4. 通过子系统推送Widget到指定栈

---

### 3. UWidget_ConfirmScreen

**职责**: 通用的确认对话框组件

**支持类型**:
```cpp
enum class EConfirmScreenType : uint8
{
    Ok,           // 单个"确定"按钮
    YesNo,        // "是"和"否"按钮
    OkCancel      // "确定"和"取消"按钮
};
```

**核心类**:

#### UConfirmScreenInfoObject
确认屏幕信息对象，用于描述对话框的内容。

```cpp
// 创建不同类型的确认屏幕
static UConfirmScreenInfoObject* CreateOKScreen(
    const FText& InScreenTitle, 
    const FText& InScreenMessage
);

static UConfirmScreenInfoObject* CreateYesNoScreen(
    const FText& InScreenTitle, 
    const FText& InScreenMessage
);

static UConfirmScreenInfoObject* CreateOKCancelScreen(
    const FText& InScreenTitle, 
    const FText& InScreenMessage
);

// 属性
UPROPERTY(Transient)
FText ScreenTitle;

UPROPERTY(Transient)
FText ScreenMessage;

UPROPERTY(Transient)
TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;
```

#### FConfirmScreenButtonInfo
按钮信息结构体。

```cpp
USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EConfirmScreenButtonType ConfirmScreenButtonType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ButtonTextToDisplay;
};
```

**使用示例**:
```cpp
// 在任意地方调用子系统推送确认对话框
UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(this);

FrontendUISubsystem->PushConfirmScreenToModalStackAsync(
    EConfirmScreenType::YesNo,
    FText::FromString("确认退出"),
    FText::FromString("确定要退出当前游戏吗？"),
    [](EConfirmScreenButtonType ClickedButtonType) {
        if (ClickedButtonType == EConfirmScreenButtonType::Confirmed)
        {
            // 用户点击了"是"，执行退出逻辑
            UGameplayStatics::OpenLevel(this, FName("MainMenu"));
        }
    }
);
```

---

## 选项设置系统

### 系统架构

选项设置系统是一个完整的游戏设置管理框架，包含以下核心组件：

```
Options System
├── Widget_OptionsScreen          # 选项主界面
├── Widget_OptionsDetailsView     # 选项详情面板
├── OptionsDataRegistry           # 数据注册表（核心）
├── OptionsDataInteractionHelper  # 数据交互辅助
├── DataObjects/                 # 数据对象（数据模型）
│   ├── ListDataObject_Base      # 基类
│   ├── ListDataObject_Collection # 集合（Tab分组）
│   ├── ListDataObject_Value     # 值类型
│   ├── ListDataObject_Scalar     # 标量（滑块）
│   ├── ListDataObject_String     # 字符串
│   ├── ListDataObject_KeyRemap   # 键位映射
│   └── ListDataObject_StringResolution # 分辨率
├── ListEntries/                 # UI条目（视图）
│   ├── Widget_ListEntry_Base
│   ├── Widget_ListEntry_KeyRemap
│   ├── Widget_ListEntry_Scalar
│   └── Widget_ListEntry_String
└── DataAsset_DataListEntryMapping  # 数据到UI映射
```

---

### 1. UWidget_OptionsScreen

**职责**: 选项设置主界面，管理所有设置选项的展示和交互

**UI结构**:
```
┌─────────────────────────────────────────────────┐
│  [视频] [音频] [控制] [游戏]    [重置] [返回]  │ ← Tab列表
├─────────────────────────────────────────────────┤
│ ┌────────────────────┬────────────────────────┐│
│ │                    │                        ││
│ │   选项列表         │    详细信息面板         ││
│ │   (ListView)       │    (DetailsView)       ││
│ │                    │                        ││
│ │  - 分辨率           │   分辨率设置          ││
│ │  - 帧率             │   调整游戏渲染帧率     ││
│ │  - 垂直同步         │                        ││
│ │  - 画质预设         │   [图像]               ││
│ │                    │                        ││
│ └────────────────────┴────────────────────────┘│
└─────────────────────────────────────────────────┘
```

**核心功能**:

1. **Tab切换管理**
   - 管理多个选项分类（视频、音频、控制、游戏）
   - 根据选中Tab动态刷新选项列表
   - Tab切换时更新重置按钮状态

2. **选项列表管理**
   - 显示当前Tab下的所有选项
   - 处理选项的选中、悬停事件
   - 追踪已修改的选项

3. **详情面板更新**
   - 实时显示选中/悬停选项的详细信息
   - 显示选项标题、描述、图标
   - 显示禁用原因（如果选项被禁用）

4. **重置功能**
   - 支持重置当前Tab所有选项
   - 显示重置提示
   - 执行批量重置操作

**关键委托**:
```cpp
// 选项界面关闭委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOptionsScreenClosed);
UPROPERTY(BlueprintAssignable, Category = "Options Screen Events")
FOnOptionsScreenClosed OnOptionsScreenClosed;
```

**生命周期**:
```cpp
// 初始化时创建数据注册表
virtual void NativeOnInitialized() override;

// 激活时初始化选项数据
virtual void NativeOnActivated() override;

// 停用时清理数据
virtual void NativeOnDeactivated() override;

// 返回初始焦点（用于手柄导航）
virtual UWidget* NativeGetDesiredFocusTarget() const override;
```

---

### 2. UOptionsDataRegistry

**职责**: 选项数据注册表，集中管理所有选项数据

**设计模式**: Registry模式（注册表模式）

**核心功能**:

1. **初始化选项树**
   ```cpp
   void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);
   ```
   - 保存本地玩家引用
   - 调用各模块初始化函数
   - 构建完整的选项数据树

2. **获取选项集合**
   ```cpp
   const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const;
   ```
   - 返回所有已注册的Tab集合
   - 按UI显示顺序返回

3. **获取扁平化列表**
   ```cpp
   TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(
       const FName& InSelectedTabID
   ) const;
   ```
   - 根据Tab ID获取该Tab下的所有选项
   - 递归遍历树形结构，返回线性列表
   - 用于ListView绑定

4. **模块初始化**
   ```cpp
   // 各Tab的初始化函数
   void InitGameplayCollectionTab();  // 游戏性选项
   void InitAudioCollectionTab();     // 音频选项
   void InitVideoCollectionTab();    // 画面选项
   void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);  // 控制选项
   ```

**数据结构示例**:
```
OptionsDataRegistry
├── Collection: "Video"
│   ├── Scalar: "Resolution"
│   ├── Scalar: "FrameRate"
│   ├── Scalar: "VSync"
│   └── Scalar: "QualityPreset"
├── Collection: "Audio"
│   ├── Scalar: "OverallVolume"
│   ├── Scalar: "MusicVolume"
│   ├── Scalar: "SoundFXVolume"
│   └── String: "AudioOutputDevice"
├── Collection: "Controls"
│   ├── KeyRemap: "Jump"
│   ├── KeyRemap: "Fire"
│   ├── KeyRemap: "MoveForward"
│   └── KeyRemap: "MoveBackward"
└── Collection: "Gameplay"
    ├── String: "Difficulty"
    └── Scalar: "HUDScale"
```

---

### 3. 选项数据对象系统

#### UListDataObject_Base

**职责**: 所有列表数据对象的基类，定义通用接口

**核心特性**:
- 使用宏 `LIST_DATA_ACCESSOR` 生成getter/setter
- 支持编辑条件
- 支持依赖关系
- 支持数据修改通知

**关键属性**:
```cpp
// 基础属性
FName DataID;                              // 数据ID
FText DataDisplayName;                     // 显示名称
FText DescriptionRichText;                 // 描述富文本
FText DisabledRichText;                    // 禁用富文本
TSoftObjectPtr<UTexture2D> SoftDescriptionImage;  // 描述图片
UListDataObject_Base* ParentData;          // 父数据
bool bShouldApplyChangeImmediately;         // 是否立即应用

// 编辑条件
TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
```

**核心委托**:
```cpp
// 数据修改委托
DECLARE_MULTICAST_DELEGATE_TwoParams(
    FOnListDataModifiedDelegate, 
    UListDataObject_Base*, 
    EOptionsListDataModifyReason
);
FOnListDataModifiedDelegate OnListDataModified;

// 依赖数据修改委托
FOnListDataModifiedDelegate OnDependencyDataModified;
```

**关键方法**:
```cpp
// 初始化数据对象
void InitDataObject();

// 设置立即应用
void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway);

// 子类需要实现的方法
virtual bool HasDefaultValue() const {return false;}
virtual bool CanResetBackToDefaultValue() const {return false;}
virtual bool TryResetBackToDefaultValue() {return false;}

// 编辑条件相关
void AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);
bool IsDataCurrentlyEditable();

// 依赖关系
void AddEditDependencyData(UListDataObject_Base* InDependencyData);
```

**子类实现示例**:
```cpp
class UListDataObject_Scalar : public UListDataObject_Base
{
public:
    // 标量特有属性
    TRange<float> DisplayValueRange;  // 显示范围
    TRange<float> OutputValueRange;   // 输出范围
    float SliderStepSize;             // 滑块步长
    ECommonNumericType DisplayNumericType;
    FCommonNumberFormattingOptions NumberedFormattingOptions;

    // 标量特有方法
    float GetCurrentValue() const;
    void SetCurrentValueFromSlider(float InNewValue);

protected:
    // 重写基类方法
    virtual bool CanResetBackToDefaultValue() const override;
    virtual bool TryResetBackToDefaultValue() override;
    virtual void OnEditDependencyDataModified(
        UListDataObject_Base* ModifiedDependencyData, 
        EOptionsListDataModifyReason ModifyReason
    ) override;
};
```

---

#### UListDataObject_Collection

**职责**: 集合类型，用于组织多个子数据对象（作为Tab或分组）

**使用场景**:
- Tab集合（如"视频"、"音频"等）
- 分组集合（如"显示设置"、"音频设置"等）

**核心方法**:
```cpp
// 添加子数据
void AddChildListData(UListDataObject_Base* InChildListData);

// 获取所有子数据
virtual TArray<UListDataObject_Base*> GetAllChildListData() const override;

// 检查是否有子数据
virtual bool HasAnyChildListData() const override;
```

---

#### UListDataObject_Scalar

**职责**: 标量数值类型，用于滑块类设置

**适用场景**:
- 音量滑块（0.0 ~ 1.0）
- 帧率限制（30 ~ 144）
- Gamma值（0.5 ~ 3.0）
- HUD缩放（0.5 ~ 2.0）

**核心特性**:
- 显示范围与输出范围分离
- 支持步长控制
- 数字格式化选项

**属性示例**:
```cpp
// 音量设置示例
DisplayValueRange = TRange<float>(0.0f, 1.0f);  // 显示0% ~ 100%
OutputValueRange = TRange<float>(0.0f, 1.0f);   // 实际值0.0 ~ 1.0
SliderStepSize = 0.05f;                        // 步长5%
DisplayNumericType = ECommonNumericType::Percentage;
NumberedFormattingOptions = FCommonNumberFormattingOptions::NoDecimal();

// 帧率设置示例
DisplayValueRange = TRange<float>(30.0f, 144.0f); // 显示30 ~ 144
OutputValueRange = TRange<float>(30.0f, 144.0f);  // 实际值30 ~ 144
SliderStepSize = 1.0f;                          // 步长1
DisplayNumericType = ECommonNumericType::Number;
NumberedFormattingOptions = FCommonNumberFormattingOptions::NoDecimal();
```

---

#### UListDataObject_KeyRemap

**职责**: 键位重映射数据对象

**核心功能**:
- 绑定新的输入键位
- 获取当前键位图标
- 重置为默认键位
- 与EnhancedInput系统集成

**关键方法**:
```cpp
// 初始化键位映射数据
void InitKeyRemapData(
    UEnhancedInputUserSettings* InOwningInputUserSettings,
    UEnhancedPlayerMappableKeyProfile* InKeyProfile,
    ECommonInputType InDesiredInputKeyType,
    const FPlayerKeyMapping& InOwningPlayerKeyMapping
);

// 获取当前键位图标
FSlateBrush GetIconFromCurrentKey() const;

// 绑定新键位
void BindNewInputKey(const FKey& InNewKey);

// 基类方法实现
virtual bool HasDefaultValue() const override;
virtual bool CanResetBackToDefaultValue() const override;
virtual bool TryResetBackToDefaultValue() override;

// 获取输入设备类型
FORCEINLINE ECommonInputType GetDesiredInputKeyType() const 
{ 
    return CachedDesiredInputKeyType; 
}
```

---

### 4. 编辑条件系统

#### FOptionsDataEditConditionDescriptor

**职责**: 描述选项的编辑条件和禁用行为

**核心功能**:
- 动态判断编辑条件
- 提供禁用原因
- 强制指定显示值

**关键方法**:
```cpp
// 设置编辑条件函数
void SetEditConditionFunc(TFunction<bool()> InEditConditionFunc);

// 判断是否满足编辑条件
bool IsEditConditionMet() const;

// 获取禁用原因
FString GetDisabledRichReason() const;

// 设置禁用原因
void SetDisabledRichReason(const FString& InDisabledRichReason);

// 检查是否有强制字符串值
bool HasForcedStringValue() const;

// 获取强制字符串值
FString GetDisabledForcedStringValue() const;

// 设置强制字符串值
void SetDisabledForcedStringValue(const FString& InDisabledForcedStringValue);
```

**使用示例**:
```cpp
// 创建一个仅在全屏模式下可用的选项
UListDataObject_Scalar* WindowModeOption = NewObject<UListDataObject_Scalar>();

FOptionsDataEditConditionDescriptor EditCondition;
EditCondition.SetEditConditionFunc([]() {
    // 检查是否在全屏模式
    return GetCurrentWindowMode() == EWindowMode::Fullscreen;
});
EditCondition.SetDisabledRichReason("<RichText>此选项仅在全屏模式下可用</RichText>");

WindowModeOption->AddEditCondition(EditCondition);
```

---

## 设置管理

### 1. UFrontendGameUserSettings

**职责**: 扩展的GameUserSettings，管理前端相关设置

**继承关系**:
```
UGameUserSettings
    ↓
UFrontendGameUserSettings
```

**核心特性**:
- Config持久化（自动保存到GameUserSettings.ini）
- 作为设置数据源
- 不直接负责应用效果

**配置示例** (GameUserSettings.ini):
```ini
[/Script/Crunch.FrontendGameUserSettings]
CurrentGameDifficulty="Normal"
OverallVolume=0.800000
MusicVolume=0.600000
SoundFXVolume=1.000000
bAllowBackgroundAudio=True
bUseHDRAudioMode=False
```

**API**:

#### 游戏性设置
```cpp
// 获取/设置游戏难度
FString GetCurrentGameDifficulty();
void SetCurrentGameDifficulty(const FString& InNewDifficulty);
```

#### 音频设置
```cpp
// 整体音量
float GetOverallVolume() const;
void SetOverallVolume(float InNewVolume);

// 音乐音量
float GetMusicVolume() const;
void SetMusicVolume(float InNewVolume);

// 音效音量
float GetSoundFXVolume() const;
void SetSoundFXVolume(float InNewVolume);

// 后台音频
bool GetAllowBackgroundAudio() const;
void SetAllowBackgroundAudio(bool bIsAllowed);

// HDR音频
bool GetUseHDRAudioMode() const;
void SetUseHDRAudioMode(bool bIsAllowed);

// 应用音频设置到音频系统
void ApplyAudioSettings();
```

#### 画面设置
```cpp
// 获取/设置Gamma值
float GetCurrentDisplayGamma() const;
void SetCurrentDisplayGamma(float InNewValue);

// 应用所有设置
virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
```

**使用示例**:
```cpp
// 获取设置实例
UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();

// 修改设置
Settings->SetOverallVolume(0.8f);
Settings->SetMusicVolume(0.6f);
Settings->SetSoundFXVolume(1.0f);

// 应用设置
Settings->ApplySettings(true);
Settings->ApplyAudioSettings();
```

---

### 2. UFrontendDeveloperSettings

**职责**: 开发者配置，定义前端UI的资源映射

**配置路径**: 项目设置 → Game → Frontend UI Settings

**核心配置**:

1. **Widget映射表**
```cpp
UPROPERTY(Config, EditAnywhere, Category = "Widget Reference")
TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> FrontendWidgetMap;
```

示例:
```ini
[Frontend.Widget.MainMenu]
WidgetClass=/Game/FrontendUI/Widgets/MainMenu/W_MainMenu

[Frontend.Widget.Options]
WidgetClass=/Game/FrontendUI/Widgets/Options/W_OptionsScreen

[Frontend.Widget.KeyRemap]
WidgetClass=/Game/FrontendUI/Widgets/Options/W_KeyRemapScreen
```

2. **选项界面图像映射**
```cpp
UPROPERTY(Config, EditAnywhere, Category = "Options Image Reference")
TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;
```

示例:
```ini
[Frontend.Image.Video]
Image=/Game/FrontendUI/Assets/Icons/Icon_Video

[Frontend.Image.Audio]
Image=/Game/FrontendUI/Assets/Icons/Icon_Audio

[Frontend.Image.Controls]
Image=/Game/FrontendUI/Assets/Icons/Icon_Controls
```

3. **音频配置**
```cpp
UPROPERTY(Config, EditAnywhere, Category = "Audio")
TSoftObjectPtr<USoundMix> MasterBusMix;

UPROPERTY(Config, EditAnywhere, Category = "Audio")
TMap<FGameplayTag, TSoftObjectPtr<USoundClass>> VolumeBusMap;
```

---

## 组件系统

### 1. UFrontendCommonListView

**职责**: 通用列表视图，自动根据数据类型生成对应的Entry Widget

**核心特性**:
- 自动数据到Widget映射
- 过滤不可选项目（如Tab集合）
- 编辑器校验

**核心方法**:
```cpp
// 内部生成列表条目（重载UCommonListView）
virtual UUserWidget& OnGenerateEntryWidgetInternal(
    UObject* Item, 
    TSubclassOf<UUserWidget> DesiredEntryClass, 
    const TSharedRef<STableViewBase>& OwnerTable
) override;

// 判断是否可选中
virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;

#if WITH_EDITOR
// 编辑器校验
virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
```

**配置属性**:
```cpp
UPROPERTY(EditAnywhere, Category = "Frontend List View Settings")
UDataAsset_DataListEntryMapping* DataListEntryMapping;
```

---

### 2. UFrontendCommonButtonBase

**职责**: 通用按钮基类

**核心特性**:
- 支持文本和图像显示
- 支持文本大写转换
- 悬停时更新描述文本

**关键方法**:
```cpp
// 设置按钮文本
void SetButtonText(FText InText);

// 获取按钮文本
FText GetButtonDisplayText() const;

// 设置按钮图像
void SetButtonDisplayImage(const FSlateBrush& InBrush);
```

**属性**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Button")
FText ButtonDisplayText;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Button")
bool bUserUpperCaseForButtonText = false;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Button")
FText ButtonDescriptionText;

UPROPERTY(meta = (BindWidgetOptional))
UCommonTextBlock* CommonTextBlock_ButtonText;

UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
UCommonLazyImage* CommonLazyImage_ButtonImage;
```

---

## 功能库

### UFrontendFunctionLibrary

**职责**: 提供前端UI相关的通用静态函数

**核心功能**:

#### Widget查找
```cpp
// 通过GameplayTag获取Widget类
static TSoftClassPtr<UWidget_ActivatableBase> GetFrontendSoftWidgetClassByTag(
    UPARAM(meta=(Categories ="Frontend.Widget")) FGameplayTag InWidgetTag
);
```

#### 图像查找
```cpp
// 通过GameplayTag获取选项界面图像
static TSoftObjectPtr<UTexture2D> GetOptionsSoftImageByTag(
    UPARAM(meta=(Categories ="Frontend.Image")) FGameplayTag InImageTag
);
```

#### 音频功能
```cpp
// 获取主音频混音器
static TSoftObjectPtr<USoundMix> GetMasterSoundMix();

// 通过GameplayTag获取SoundClass
static TSoftObjectPtr<USoundClass> GetSoundClassByTag(
    UPARAM(meta=(Categories ="Frontend.Audio")) FGameplayTag InVolumeTag
);
```

#### 输入功能
```cpp
// 根据InputAction获取按键图标
static FSlateBrush GetInputActionIcon(
    const FName MappingName,
    const ULocalPlayer* InLocalPlayer,
    ECommonInputType DesiredInputType
);
```

**使用示例**:
```cpp
// 获取Widget类
TSoftClassPtr<UWidget_ActivatableBase> OptionsWidgetClass = 
    UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(
        FGameplayTag::RequestGameplayTag(FName("Frontend.Widget.Options"))
    );

// 获取图标
TSoftObjectPtr<UTexture2D> VideoIcon = 
    UFrontendFunctionLibrary::GetOptionsSoftImageByTag(
        FGameplayTag::RequestGameplayTag(FName("Frontend.Image.Video"))
    );

// 获取按键图标
FSlateBrush JumpIcon = UFrontendFunctionLibrary::GetInputActionIcon(
    FName("Jump"),
    GetLocalPlayer(),
    ECommonInputType::KeyboardMouse
);
```

---

## 异步操作

### 1. UAsyncAction_PushSoftWidget

**职责**: 异步加载并推送Widget到UI栈

**使用场景**:
- 延迟加载非即时需要的Widget
- 避免主界面启动时的资源加载压力
- 动态加载内容

**流程**:
```
1. 接收软引用Widget类
2. 异步加载Widget类资源
3. 创建Widget实例
4. 初始化Widget
5. 推送到指定UI栈
6. 回调通知完成
```

---

### 2. UAsyncAction_PushConfirmScreen

**职责**: 异步创建并推送确认对话框

**支持类型**:
- `EConfirmScreenType::Ok` - 单按钮确认
- `EConfirmScreenType::YesNo` - 是/否选择
- `EConfirmScreenType::OkCancel` - 确定/取消选择

**使用示例**:
```cpp
// 通过子系统调用
UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
    EConfirmScreenType::YesNo,
    FText::FromString("保存更改"),
    FText::FromString("确定要保存所有更改吗？"),
    [](EConfirmScreenButtonType ClickedButtonType) {
        if (ClickedButtonType == EConfirmScreenButtonType::Confirmed)
        {
            // 保存逻辑
            UFrontendGameUserSettings::Get()->SaveSettings();
        }
    }
);
```

---

## 类型定义

### 枚举类型 (FrontendEnumTypes.h)

```cpp
// 确认屏幕类型
UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
    Ok UMETA(DisplayName = "确定"),
    YesNo UMETA(DisplayName = "是/否"),
    OkCancel UMETA(DisplayName = "确定/取消"),
    Unknown UMETA(Hidden)
};

// 确认屏幕按钮类型
UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
    Confirmed UMETA(DisplayName = "确认"),
    Cancelled UMETA(DisplayName = "取消"),
    Closed UMETA(DisplayName = "关闭"),
    Unknown UMETA(Hidden)
};

// 选项数据修改原因
UENUM(BlueprintType)
enum class EOptionsListDataModifyReason : uint8
{
    DirectlyModified UMETA(DisplayName = "直接修改"),
    DependencyModified UMETA(DisplayName = "依赖项变更"),
    ResetToDefault UMETA(DisplayName = "重置为默认值")
};
```

---

## 设计模式

### 1. 注册表模式 (Registry Pattern)

**实现**: `UOptionsDataRegistry`

**作用**: 集中管理和注册所有选项数据，提供统一的访问接口

**优势**:
- 集中管理，易于维护
- 统一的数据访问接口
- 支持动态注册和查找

---

### 2. 模板方法模式 (Template Method)

**实现**: `UListDataObject_Base`

**作用**: 定义算法骨架，子类实现具体步骤

**示例**:
```cpp
// 基类定义骨架
class UListDataObject_Base {
    void InitDataObject() {
        // 通用初始化逻辑
        OnDataObjectInitialized();  // 调用子类实现
    }
    
    virtual void OnDataObjectInitialized();  // 子类实现
};

// 子类实现具体逻辑
class UListDataObject_Scalar : public UListDataObject_Base {
    void OnDataObjectInitialized() override {
        // Scalar特有的初始化
    }
};
```

---

### 3. 观察者模式 (Observer Pattern)

**实现**: 选项数据修改委托

**作用**: 当数据变化时通知所有监听者

**示例**:
```cpp
// 定义委托
DECLARE_MULTICAST_DELEGATE_TwoParams(
    FOnListDataModifiedDelegate, 
    UListDataObject_Base*, 
    EOptionsListDataModifyReason
);

// 触发委托
void UListDataObject_Base::NotifyListDataModified(
    UListDataObject_Base* ModifiedData, 
    EOptionsListDataModifyReason ModifyReason)
{
    OnListDataModified.Broadcast(ModifiedData, ModifyReason);
}

// 监听委托
DataObject->OnListDataModified.AddLambda([](UListDataObject_Base* Modified, EOptionsListDataModifyReason Reason) {
    // 处理数据修改
});
```

---

### 4. 工厂模式 (Factory Pattern)

**实现**: `UConfirmScreenInfoObject`

**作用**: 根据不同需求创建不同类型的确认对话框

**示例**:
```cpp
// 工厂方法
static UConfirmScreenInfoObject* CreateOKScreen(...);
static UConfirmScreenInfoObject* CreateYesNoScreen(...);
static UConfirmScreenInfoObject* CreateOKCancelScreen(...);

// 使用
auto Info = UConfirmScreenInfoObject::CreateYesNoScreen(Title, Message);
```

---

### 5. 策略模式 (Strategy Pattern)

**实现**: 编辑条件函数

**作用**: 动态切换不同的编辑条件判断策略

**示例**:
```cpp
// 定义不同的策略
auto Strategy1 = []() { return Condition1; };
auto Strategy2 = []() { return Condition2; };

// 动态切换
EditCondition.SetEditConditionFunc(UseStrategy1 ? Strategy1 : Strategy2);
```

---

## 最佳实践

### 1. Widget创建流程

```cpp
// 1. 在开发者设置中配置Widget映射
FrontendDeveloperSettings->FrontendWidgetMap.Add(
    FGameplayTag::RequestGameplayTag("Frontend.Widget.MyCustomWidget"),
    TSoftClassPtr<UWidget_ActivatableBase>(UMyCustomWidget::StaticClass())
);

// 2. 通过子系统推送Widget
UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
    FGameplayTag::RequestGameplayTag("Frontend.WidgetStack.MainStack"),
    UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(
        FGameplayTag::RequestGameplayTag("Frontend.Widget.MyCustomWidget")
    ),
    [](EAsyncPushWidgetState State, UWidget_ActivatableBase* Widget) {
        if (State == EAsyncPushWidgetState::AfterPush) {
            // Widget已推送到栈，可以执行后续逻辑
        }
    }
);
```

---

### 2. 选项数据创建流程

```cpp
// 创建选项数据对象
UListDataObject_Scalar* VolumeOption = NewObject<UListDataObject_Scalar>(this);
VolumeOption->InitDataObject();
VolumeOption->SetDataDisplayName(FText::FromString("音量"));
VolumeOption->SetDescriptionRichText(FText::FromString("调整游戏音量"));
VolumeOption->SetDisplayValueRange(TRange<float>(0.0f, 1.0f));
VolumeOption->SetOutputValueRange(TRange<float>(0.0f, 1.0f));
VolumeOption->SetSliderStepSize(0.05f);

// 设置当前值
VolumeOption->SetCurrentValueFromSlider(0.8f);

// 监听修改
VolumeOption->OnListDataModified.AddLambda([](UListDataObject_Base* Modified, EOptionsListDataModifyReason Reason) {
    // 处理音量修改
    if (auto* Scalar = Cast<UListDataObject_Scalar>(Modified)) {
        float Volume = Scalar->GetCurrentValue();
        // 应用音量设置
    }
});

// 添加到集合
Collection->AddChildListData(VolumeOption);
```

---

### 3. 确认对话框使用

```cpp
// 简单确认对话框
UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
    EConfirmScreenType::Ok,
    FText::FromString("提示"),
    FText::FromString("操作成功！"),
    [](EConfirmScreenButtonType ClickedButtonType) {
        // 点击确定后的处理
    }
);

// 是/否对话框
UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
    EConfirmScreenType::YesNo,
    FText::FromString("确认"),
    FText::FromString("确定要执行此操作吗？"),
    [](EConfirmScreenButtonType ClickedButtonType) {
        if (ClickedButtonType == EConfirmScreenButtonType::Confirmed) {
            // 用户点击"是"
        } else if (ClickedButtonType == EConfirmScreenButtonType::Cancelled) {
            // 用户点击"否"
        }
    }
);
```

---

### 4. 设置持久化

```cpp
// 获取设置实例
UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();

// 修改设置
Settings->SetOverallVolume(0.8f);
Settings->SetMusicVolume(0.6f);
Settings->SetSoundFXVolume(1.0f);

// 应用设置
Settings->ApplySettings(true);  // bCheckForCommandLineOverrides
Settings->ApplyAudioSettings();  // 应用到音频系统

// 保存设置（自动保存到.ini）
Settings->SaveSettings();
```

---

### 5. 自定义选项数据类型

```cpp
// 1. 继承基类
UCLASS()
class UMyCustomListDataObject : public UListDataObject_Base
{
    GENERATED_BODY()

public:
    // 添加自定义属性
    LIST_DATA_ACCESSOR(FString, CustomStringValue)

    // 重写基类方法
    virtual bool HasDefaultValue() const override
    {
        return CustomStringValue == "Default";
    }

    virtual bool CanResetBackToDefaultValue() const override
    {
        return HasDefaultValue();
    }

    virtual bool TryResetBackToDefaultValue() override
    {
        CustomStringValue = "Default";
        NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
        return true;
    }

protected:
    virtual void OnDataObjectInitialized() override
    {
        // 自定义初始化逻辑
        SetDataDisplayName(FText::FromString("自定义选项"));
    }

private:
    FString CustomStringValue;
};

// 2. 创建对应的Entry Widget
UCLASS()
class UMyCustomListEntry : public UWidget_ListEntry_Base
{
    GENERATED_BODY()

public:
    void SetListItemObjectInternal(UObject* InListItem) override
    {
        Super::SetListItemObjectInternal(InListItem);
        
        if (auto* CustomData = Cast<UMyCustomListDataObject>(InListItem))
        {
            // 绑定数据到UI
            TextBlock_Value->SetText(FText::FromString(CustomData->GetCustomStringValue()));
        }
    }
};

// 3. 在映射表中添加映射
DataListEntryMapping->AddMapping(
    UMyCustomListDataObject::StaticClass(),
    UMyCustomListEntry::StaticClass()
);
```

---

## 性能优化

### 1. 软引用使用

使用软引用延迟加载资源，避免启动时加载过多资源。

```cpp
// ✅ 推荐：使用软引用
TSoftClassPtr<UWidget_ActivatableBase> WidgetClass;
TSoftObjectPtr<UTexture2D> SoftImage;

// ❌ 不推荐：硬引用（会立即加载）
UClass* WidgetClass;  // 会立即加载
UTexture2D* Image;     // 会立即加载
```

---

### 2. Transient标记

标记不需要序列化的属性为Transient，避免不必要的数据保存。

```cpp
UPROPERTY(Transient)
UWidget_PrimaryLayout* CreatedPrimaryLayout;  // 运行时创建，不需要保存

UPROPERTY(Transient)
TArray<UListDataObject_Base*> ResettableDataArray;  // 临时数据，不需要保存
```

---

### 3. 禁用默认Tick

对于不需要每帧更新的Widget，禁用默认Tick。

```cpp
UCLASS(meta = (DisableNaiveTick))
class UMyWidget : public UWidget_ActivatableBase
{
    // ...
};
```

---

### 4. 异步加载

使用异步操作加载资源，避免阻塞主线程。

```cpp
// ✅ 推荐：异步加载
UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(...);

// ❌ 不推荐：同步加载（可能卡顿）
LoadClass<UWidget_ActivatableBase>(nullptr, *WidgetPath);
```

---

## 调试技巧

### 1. Widget栈调试

查看当前UI栈中的Widget。

```cpp
// 在选项界面中
UCommonActivatableWidgetContainerBase* WidgetStack = PrimaryLayout->FindWidgetStackByTag(
    FGameplayTag::RequestGameplayTag("Frontend.WidgetStack.MainStack")
);

// 遍历栈中Widget
for (auto* Widget : WidgetStack->GetActiveWidgets())
{
    UE_LOG(LogTemp, Warning, TEXT("Stack Widget: %s"), *Widget->GetName());
}
```

---

### 2. 选项数据调试

查看选项数据的当前状态。

```cpp
// 在选项界面中
for (auto* DataObj : ResettableDataArray)
{
    UE_LOG(LogTemp, Warning, TEXT("Option: %s, Modified: %s"),
        *DataObj->GetDataDisplayName().ToString(),
        DataObj->HasDefaultValue() ? TEXT("No") : TEXT("Yes")
    );
}
```

---

### 3. 设置调试

查看当前设置值。

```cpp
UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();

UE_LOG(LogTemp, Warning, TEXT("OverallVolume: %f"), Settings->GetOverallVolume());
UE_LOG(LogTemp, Warning, TEXT("MusicVolume: %f"), Settings->GetMusicVolume());
UE_LOG(LogTemp, Warning, TEXT("SoundFXVolume: %f"), Settings->GetSoundFXVolume());
UE_LOG(LogTemp, Warning, TEXT("Gamma: %f"), Settings->GetCurrentDisplayGamma());
```

---

## 扩展指南

### 1. 添加新的Widget

```cpp
// 1. 创建Widget类
UCLASS()
class UMyNewWidget : public UWidget_ActivatableBase
{
    GENERATED_BODY()

protected:
    virtual void NativeOnActivated() override
    {
        Super::NativeOnActivated();
        // 激活逻辑
    }
};

// 2. 在开发者设置中映射
FrontendDeveloperSettings->FrontendWidgetMap.Add(
    FGameplayTag::RequestGameplayTag("Frontend.Widget.MyNewWidget"),
    TSoftClassPtr<UWidget_ActivatableBase>(UMyNewWidget::StaticClass())
);

// 3. 使用子系统推送
UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
    FGameplayTag::RequestGameplayTag("Frontend.WidgetStack.MainStack"),
    UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(
        FGameplayTag::RequestGameplayTag("Frontend.Widget.MyNewWidget")
    ),
    [](EAsyncPushWidgetState State, UWidget_ActivatableBase* Widget) {
        // 完成
    }
);
```

---

### 2. 添加新的选项数据类型

参见[最佳实践 - 5. 自定义选项数据类型](#5-自定义选项数据类型)

---

### 3. 添加新的选项分类

```cpp
// 在UOptionsDataRegistry中添加新的初始化函数
void UOptionsDataRegistry::InitMyCustomCollectionTab()
{
    // 创建Collection
    UListDataObject_Collection* MyCustomCollection = NewObject<UListDataObject_Collection>(this);
    MyCustomCollection->InitDataObject();
    MyCustomCollection->SetDataDisplayName(FText::FromString("自定义"));
    MyCustomCollection->SetDataID(FName("MyCustom"));

    // 添加子选项
    UListDataObject_Scalar* Option1 = NewObject<UListDataObject_Scalar>(this);
    Option1->InitDataObject();
    Option1->SetDataDisplayName(FText::FromString("选项1"));
    // ... 配置选项1

    UListDataObject_Scalar* Option2 = NewObject<UListDataObject_Scalar>(this);
    Option2->InitDataObject();
    Option2->SetDataDisplayName(FText::FromString("选项2"));
    // ... 配置选项2

    MyCustomCollection->AddChildListData(Option1);
    MyCustomCollection->AddChildListData(Option2);

    // 注册到集合列表
    RegisteredOptionsTabCollections.Add(MyCustomCollection);
}

// 在InitOptionsDataRegistry中调用
void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
    OwningLocalPlayer = InOwningLocalPlayer;

    InitGameplayCollectionTab();
    InitAudioCollectionTab();
    InitVideoCollectionTab();
    InitControlCollectionTab(InOwningLocalPlayer);
    InitMyCustomCollectionTab();  // 添加新分类
}
```

---

## 总结

Frontend UI 系统提供了一个功能完整、架构清晰的前端界面框架。通过模块化设计、数据驱动和异步加载等技术，实现了高效、可扩展的UI系统。

**核心组件**:
- **子系统**: UFrontendUISubsystem - 核心管理
- **Widget基类**: UWidget_ActivatableBase - 统一基类
- **选项系统**: UOptionsDataRegistry + 数据对象 - 完整设置框架
- **设置管理**: UFrontendGameUserSettings - 配置持久化
- **功能库**: UFrontendFunctionLibrary - 通用工具

**设计模式**:
- 注册表模式
- 模板方法模式
- 观察者模式
- 工厂模式
- 策略模式

通过遵循本文档的最佳实践和扩展指南，可以轻松添加新的UI功能和选项设置。
