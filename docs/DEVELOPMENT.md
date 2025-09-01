# Crunch 开发指南

## 🛠️ 开发环境搭建

### 必需软件
- **Unreal Engine 5.4+**: [下载地址](https://www.unrealengine.com/)
- **Visual Studio 2022**: 包含C++开发工具
- **Git**: 版本控制
- **JetBrains Rider** (可选): 推荐的C++IDE

### 推荐插件
- **UnrealVS**: Visual Studio的UE集成插件
- **RiderLink**: 已集成在项目中
- **Visual Assist**: C++代码补全和导航

## 📝 代码规范

### 命名约定

#### 类命名
```cpp
// UObject派生类使用U前缀
class UInventoryComponent : public UActorComponent {};

// Actor派生类使用A前缀  
class ACrunchCharacter : public ACharacter {};

// 结构体使用F前缀
struct FPlayerSelection {};

// 枚举使用E前缀
enum class ETeamSide : uint8 {};

// 接口使用I前缀
class ITreeNodeInterface {};
```

#### 变量命名
```cpp
// 成员变量使用驼峰命名
int32 PlayerHealth;
bool bIsPlayerAlive;  // 布尔值使用b前缀

// 常量使用全大写+下划线
static const float MAX_PLAYER_SPEED = 600.0f;

// 临时变量使用描述性名称
for (const auto& Character : AllCharacters)
{
    // 处理角色...
}
```

#### 函数命名
```cpp
// 公共函数使用动词开头
void UpdatePlayerHealth(float NewHealth);
bool CanPlayerMove() const;
float GetPlayerSpeed() const;

// Blueprint可调用函数添加前缀
UFUNCTION(BlueprintCallable)
void BP_SetPlayerTeam(int32 TeamID);

// 事件处理函数使用On前缀
void OnPlayerDied();
void OnItemPurchased(const UPDA_ShopItem* Item);
```

### 文件组织

#### 头文件结构
```cpp
// 文件头注释
// 幻雨喜欢小猫咪

#pragma once

// 引擎头文件
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// 项目头文件
#include "Player/PlayerInfoTypes.h"
#include "GAS/Core/CAttributeSet.h"

// 生成的头文件(必须最后)
#include "CrunchCharacter.generated.h"

// 前置声明
class UInventoryComponent;
class UAbilitySystemComponent;

/**
 * 类的详细文档注释
 * 说明类的作用和使用方法
 */
UCLASS(BlueprintType)
class CRUNCH_API ACrunchCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 构造函数
    ACrunchCharacter();

    // 公共接口函数
    // ...

protected:
    // 受保护的函数和变量
    // ...

private:
    // 私有成员
    // ...
};
```

#### 实现文件结构
```cpp
#include "Character/CrunchCharacter.h"

// 引擎包含
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// 项目包含
#include "Inventory/InventoryComponent.h"
#include "GAS/CAbilitySystemComponent.h"

ACrunchCharacter::ACrunchCharacter()
{
    // 构造函数实现
}

void ACrunchCharacter::BeginPlay()
{
    Super::BeginPlay();
    // BeginPlay实现
}
```

## 🎮 核心系统开发

### 角色系统开发

#### 创建新角色
1. **创建角色定义数据资产**
```cpp
// 在编辑器中创建新的CharacterDefinition资产
// Content/Characters/Definitions/DA_NewCharacter
```

2. **配置角色属性**
```cpp
// 在数据资产中设置:
// - 角色网格体
// - 动画蓝图
// - 初始技能
// - 属性值
```

3. **测试角色**
```cpp
// 在角色选择界面测试新角色
// 确保所有动画和技能正常工作
```

#### 角色技能开发
```cpp
// 1. 创建技能类
UCLASS()
class CRUNCH_API UGA_NewAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;
};

// 2. 实现技能逻辑
void UGA_NewAbility::ActivateAbility(...)
{
    // 检查技能条件
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 执行技能效果
    // ...

    // 结束技能
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
```

### UI系统开发

#### 创建新的UI控件
1. **创建C++基类**
```cpp
UCLASS()
class CRUNCH_API UNewWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

private:
    // 绑定UI元素
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ActionButton;

    UPROPERTY(meta = (BindWidget)) 
    TObjectPtr<UTextBlock> InfoText;

    // 事件处理
    UFUNCTION()
    void OnActionButtonClicked();
};
```

2. **创建对应的蓝图**
```cpp
// 在编辑器中:
// 1. 创建基于UNewWidget的蓝图类
// 2. 设计UI布局
// 3. 绑定控件名称与C++变量匹配
```

3. **集成到游戏中**
```cpp
// 在适当的地方显示UI
if (NewWidgetClass)
{
    auto NewWidget = CreateWidget<UNewWidget>(this, NewWidgetClass);
    NewWidget->AddToViewport();
}
```

#### UI绑定最佳实践
```cpp
// ✅ 正确的绑定方式
UPROPERTY(meta = (BindWidget))
TObjectPtr<UButton> StartButton;

// ✅ 绑定事件
virtual void NativeConstruct() override
{
    Super::NativeConstruct();
    
    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &ThisClass::OnStartButtonClicked);
    }
}

// ❌ 错误的做法 - 不检查nullptr
void BadExample()
{
    StartButton->SetIsEnabled(false); // 可能崩溃
}
```

### 聊天系统开发

#### 创建聊天消息处理
1. **实现聊天接口**
```cpp
// 在PlayerController中实现IChatInterface
class CRUNCH_API ACPlayerController : public APlayerController, public IChatInterface
{
public:
    // 实现接口方法
    virtual void ReceiveChatMessageFromServer(const FChatMessage& Message) override;
    virtual void SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType) override;

    // 网络RPC函数
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);

    UFUNCTION(Client, Reliable)
    void Client_ReceiveChatMessage(const FChatMessage& Message);
};
```

2. **实现网络RPC函数**
```cpp
void ACPlayerController::Server_SendChatMessage_Implementation(
    const FString& Message, EChatChannelType ChannelType)
{
    // 验证消息内容
    if (Message.IsEmpty() || Message.Len() > 256)
    {
        return;
    }

    // 获取发送者名称
    FString SenderName = GetPlayerState<APlayerState>()->GetPlayerName();
    if (SenderName.IsEmpty())
    {
        SenderName = TEXT("Unknown Player");
    }

    // 创建消息对象
    FChatMessage ChatMessage(SenderName, Message, ChannelType, GetGenericTeamId());

    // 根据频道类型决定发送范围
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ACPlayerController> It(World); It; ++It)
    {
        ACPlayerController* PlayerController = *It;
        if (!PlayerController) continue;

        // 全体聊天：发送给所有玩家
        if (ChannelType == EChatChannelType::All)
        {
            PlayerController->Client_ReceiveChatMessage(ChatMessage);
        }
        // 队友聊天：只发送给同队伍玩家
        else if (ChannelType == EChatChannelType::Team)
        {
            if (PlayerController->GetGenericTeamId() == GetGenericTeamId())
            {
                PlayerController->Client_ReceiveChatMessage(ChatMessage);
            }
        }
    }
}

bool ACPlayerController::Server_SendChatMessage_Validate(
    const FString& Message, EChatChannelType ChannelType)
{
    return !Message.IsEmpty() && Message.Len() <= 256;
}
```

#### 聊天UI开发
1. **创建聊天控件**
```cpp
// 创建新的聊天控件
UCLASS()
class CRUNCH_API UMyChatWidget : public UChatWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // 自定义消息处理
    UFUNCTION(BlueprintCallable)
    void CustomMessageHandler(const FChatMessage& Message);

protected:
    // 添加自定义UI元素
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> EmojiButton;

    UFUNCTION()
    void OnEmojiButtonClicked();
};
```

2. **集成到游戏中**
```cpp
// 在适当的地方显示ChatWidget
void ACPlayerController::ToggleChat()
{
    if (GameplayWidget)
    {
        // 使用智能切换：在临时模式下直接进入正常聊天，否则正常切换
        GameplayWidget->SmartToggleChat();
    }
}

// 设置输入绑定
void ACPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    // 绑定聊天快捷键
    InputComponent->BindAction("OpenChat", IE_Pressed, this, &ACPlayerController::ToggleChat);
}
```

#### 弹幕系统开发
```cpp
// 创建弹幕消息
void UGameplayWidget::ShowBarrageMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate)
{
    if (!GameplayWidgetRootPanel || !ChatMessageItemClass)
    {
        return;
    }

    // 创建弹幕消息控件
    auto BarrageWidget = CreateWidget<UChatMessageItemWidget>(this, ChatMessageItemClass);
    if (!BarrageWidget)
    {
        return;
    }

    // 设置弹幕位置和动画
    GameplayWidgetRootPanel->AddChild(BarrageWidget);
    BarrageWidget->SetAsBarrageMode(Message, bIsSelf, bIsTeammate, GameplayWidgetRootPanel);
}
```

### 网络开发

#### 创建可复制的Actor
```cpp
UCLASS()
class CRUNCH_API ANetworkedActor : public AActor
{
    GENERATED_BODY()

public:
    ANetworkedActor();

    // 设置复制属性
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    // 复制的变量
    UPROPERTY(Replicated)
    int32 Health;

    UPROPERTY(ReplicatedUsing = OnRep_PlayerName)
    FString PlayerName;

    // 复制回调
    UFUNCTION()
    void OnRep_PlayerName();

    // 服务器RPC
    UFUNCTION(Server, Reliable)
    void Server_TakeDamage(int32 Damage);

    // 客户端RPC  
    UFUNCTION(Client, Reliable)
    void Client_ShowDamageEffect();
};
```

#### 实现网络函数
```cpp
void ANetworkedActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ANetworkedActor, Health);
    DOREPLIFETIME(ANetworkedActor, PlayerName);
}

void ANetworkedActor::Server_TakeDamage_Implementation(int32 Damage)
{
    Health -= Damage;
    
    // 通知所有客户端
    if (Health <= 0)
    {
        Client_ShowDamageEffect();
    }
}
```

## 🧪 测试与调试

### 单元测试
```cpp
// 创建测试类
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCrunchInventoryTest, "Crunch.Inventory.BasicFunctionality", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCrunchInventoryTest::RunTest(const FString& Parameters)
{
    // 创建测试组件
    UInventoryComponent* TestInventory = NewObject<UInventoryComponent>();
    
    // 测试添加物品
    bool bAddResult = TestInventory->AddItem(TestItem, 5);
    TestTrue("Should be able to add items", bAddResult);
    
    // 测试物品数量
    int32 ItemCount = TestInventory->GetItemCount(TestItem);
    TestEqual("Item count should be 5", ItemCount, 5);
    
    return true;
}
```

### 网络测试
```cpp
// 在编辑器中测试多人游戏
// Play -> Advanced Settings -> 
// - Number of Players: 2+
// - Run Under One Process: false
// - Editor Multiplayer Mode: Play as Listen Server
```

### 性能分析
```cpp
// 使用性能分析工具
// 控制台命令:
stat fps          // 显示帧率
stat unit         // 显示各个线程耗时
stat memory       // 显示内存使用
stat networking   // 显示网络统计

// 代码中添加性能标记
SCOPE_CYCLE_COUNTER(STAT_InventoryUpdate);
void UInventoryComponent::UpdateInventory()
{
    // 函数实现...
}
```

## 🔧 调试技巧

### 日志使用
```cpp
// 在模块中定义日志类别
DEFINE_LOG_CATEGORY(LogCrunchInventory);

// 使用不同级别的日志
UE_LOG(LogCrunchInventory, Log, TEXT("Item added: %s"), *ItemName);
UE_LOG(LogCrunchInventory, Warning, TEXT("Inventory full!"));
UE_LOG(LogCrunchInventory, Error, TEXT("Failed to add item: %s"), *Error);

// 带格式化的日志
UE_LOG(LogCrunchInventory, Log, TEXT("Player %s bought %d items for %f gold"), 
    *PlayerName, ItemCount, TotalPrice);
```

### 断点调试
```cpp
// 条件断点
if (PlayerHealth <= 0)
{
    UE_LOG(LogTemp, Warning, TEXT("Player died!")); // 在这里设置断点
}

// 使用check和ensure
check(PlayerController != nullptr); // Debug版本中断言
ensure(IsValid(TargetActor));       // 即使在Release版本也检查
```

### Blueprint调试
- 使用Print String节点输出调试信息
- 设置断点观察变量值
- 使用Debug Filter查看特定对象

## 📚 最佳实践

### 性能优化
```cpp
// ✅ 缓存频繁访问的组件
void ACrunchCharacter::BeginPlay()
{
    Super::BeginPlay();
    CachedInventoryComponent = GetComponentByClass<UInventoryComponent>();
}

// ✅ 使用对象池避免频繁创建销毁
TArray<AProjectile*> ProjectilePool;

// ✅ 合理使用Tick
virtual void Tick(float DeltaTime) override
{
    // 只在必要时才Tick
    if (bNeedsTick)
    {
        Super::Tick(DeltaTime);
        // Tick逻辑...
    }
}
```

### 内存管理
```cpp
// ✅ 使用TObjectPtr管理UObject引用
UPROPERTY()
TObjectPtr<UInventoryComponent> InventoryComponent;

// ✅ 及时清理委托绑定
virtual void BeginDestroy() override
{
    if (SomeDelegate.IsValid())
    {
        SomeDelegate.Unbind();
    }
    Super::BeginDestroy();
}

// ✅ 使用智能指针管理非UObject
TSharedPtr<FComplexData> ComplexDataPtr;
```

### 错误处理
```cpp
// ✅ 检查指针有效性
if (IsValid(TargetActor))
{
    TargetActor->TakeDamage(DamageAmount);
}

// ✅ 使用Optional处理可能失败的操作
TOptional<FVector> GetPlayerLocation(int32 PlayerID)
{
    if (auto* Player = FindPlayerByID(PlayerID))
    {
        return Player->GetActorLocation();
    }
    return {};
}

// 使用方式
if (auto Location = GetPlayerLocation(PlayerID))
{
    // 使用Location.GetValue()
}
```

## 🔄 版本控制

### Git工作流
```bash
# 功能分支开发
git checkout -b feature/new-character-system
git add .
git commit -m "Add: 新角色系统基础框架"
git push origin feature/new-character-system

# 合并到主分支
git checkout main
git merge feature/new-character-system
git push origin main
```

### 提交信息规范
```
类型: 简要描述

详细描述 (可选)

类型包括:
- Add: 新增功能
- Fix: 修复bug
- Update: 更新现有功能
- Remove: 删除代码
- Refactor: 重构代码
- Docs: 文档更新
```

这个开发指南为Crunch项目的开发者提供了详细的开发流程和最佳实践，确保代码质量和团队协作效率。