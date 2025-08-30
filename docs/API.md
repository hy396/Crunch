# Crunch API 文档

## 📋 概览

这个文档描述了Crunch游戏项目中主要类和接口的API。所有API都是基于Unreal Engine 5的框架构建的。

## 🎮 核心游戏框架

### UMGameInstance

游戏实例管理类，负责整个游戏的生命周期管理。

```cpp
class CRUNCH_API UMGameInstance : public UGameInstance
{
public:
    /**
     * 创建游戏会话
     * @param SessionName 会话名称
     * @param MaxPlayers 最大玩家数
     * @return 是否成功创建
     */
    UFUNCTION(BlueprintCallable, Category = "Session")
    bool CreateGameSession(const FString& SessionName, int32 MaxPlayers = 8);

    /**
     * 加入游戏会话
     * @param SessionId 会话ID
     * @return 是否成功加入
     */
    UFUNCTION(BlueprintCallable, Category = "Session")
    bool JoinGameSession(const FString& SessionId);

    /**
     * 搜索可用会话
     * @param OnComplete 完成回调
     */
    UFUNCTION(BlueprintCallable, Category = "Session")
    void FindGameSessions(const FOnSessionSearchComplete& OnComplete);

    /**
     * 离开当前会话
     */
    UFUNCTION(BlueprintCallable, Category = "Session")
    void LeaveSession();

    /**
     * 获取当前玩家数据
     * @return 玩家状态信息
     */
    UFUNCTION(BlueprintPure, Category = "Player")
    FPlayerSelection GetCurrentPlayerSelection() const;

    /**
     * 设置玩家选择的角色
     * @param CharacterDefinition 角色定义
     */
    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetSelectedCharacter(const UPDA_CharacterDefinition* CharacterDefinition);
};
```

### ACrunchPlayerController

玩家控制器，处理玩家输入和网络通信。

```cpp
class CRUNCH_API ACrunchPlayerController : public APlayerController
{
public:
    /**
     * 设置玩家队伍
     * @param NewTeamID 队伍ID (1或2)
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Team")
    void Server_SetPlayerTeam(uint8 NewTeamID);

    /**
     * 请求角色选择
     * @param CharacterDefinition 选择的角色
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Character")
    void Server_SelectCharacter(const UPDA_CharacterDefinition* CharacterDefinition);

    /**
     * 获取玩家昵称
     * @return 玩家名称
     */
    UFUNCTION(BlueprintPure, Category = "Player")
    FString GetPlayerNickname() const;

    /**
     * 显示游戏UI
     * @param WidgetClass UI类
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowGameWidget(TSubclassOf<UUserWidget> WidgetClass);

    /**
     * 隐藏游戏UI
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideGameWidget();
};
```

## 🧙‍♂️ 角色系统

### UPDA_CharacterDefinition

角色定义数据资产，包含角色的所有配置信息。

```cpp
UCLASS(BlueprintType)
class CRUNCH_API UPDA_CharacterDefinition : public UPrimaryDataAsset
{
public:
    /** 角色显示名称 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    FText CharacterName;

    /** 角色描述 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    FText CharacterDescription;

    /** 角色图标 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    TObjectPtr<UTexture2D> CharacterIcon;

    /** 角色网格体 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    TObjectPtr<USkeletalMesh> CharacterMesh;

    /** 动画蓝图 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
    TSubclassOf<UAnimInstance> AnimBlueprint;

    /** 初始技能列表 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

    /** 初始属性效果 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
    TArray<TSubclassOf<UGameplayEffect>> StartingEffects;

    /**
     * 获取角色的基础属性值
     * @param AttributeName 属性名称
     * @return 属性值
     */
    UFUNCTION(BlueprintPure, Category = "Character")
    float GetBaseAttributeValue(const FString& AttributeName) const;
};
```

### ACrunchCharacter

游戏角色基类。

```cpp
class CRUNCH_API ACrunchCharacter : public ACharacter
{
public:
    ACrunchCharacter();

    /**
     * 使用角色定义配置角色
     * @param CharacterDefinition 角色定义数据
     */
    UFUNCTION(BlueprintCallable, Category = "Character")
    void ConfigureWithCharacterDefinition(const UPDA_CharacterDefinition* CharacterDefinition);

    /**
     * 获取角色的库存组件
     * @return 库存组件引用
     */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

    /**
     * 获取技能系统组件
     * @return ASC组件引用
     */
    UFUNCTION(BlueprintPure, Category = "Abilities")
    UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

    /**
     * 角色死亡处理
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Character")
    void OnCharacterDied();

    /**
     * 角色复活
     */
    UFUNCTION(BlueprintCallable, Category = "Character")
    void Respawn();

protected:
    /** 库存组件 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    /** 技能系统组件 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    /** 属性集 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCAttributeSet> AttributeSet;
};
```

## ⚔️ 技能系统 (GAS)

### UCAttributeSet

角色属性集，定义所有游戏属性。

```cpp
class CRUNCH_API UCAttributeSet : public UAttributeSet
{
public:
    /** 生命值 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Health)

    /** 最大生命值 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth)

    /** 魔法值 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana)

    /** 最大魔法值 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana)

    /** 攻击力 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackPower)

    /** 移动速度 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MovementSpeed)
    FGameplayAttributeData MovementSpeed;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MovementSpeed)

    /** 角色等级 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CharacterLevel)
    FGameplayAttributeData CharacterLevel;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, CharacterLevel)

    /** 经验值 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_XP)
    FGameplayAttributeData XP;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, XP)

    /** 技能点 */
    UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AbilityPoints)
    FGameplayAttributeData AbilityPoints;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, AbilityPoints)

protected:
    // 属性复制回调函数
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

    // ... 其他属性的OnRep函数
};
```

### UGameplayAbility_Base

技能基类。

```cpp
class CRUNCH_API UGameplayAbility_Base : public UGameplayAbility
{
public:
    /**
     * 获取技能冷却时间
     * @return 冷却时间（秒）
     */
    UFUNCTION(BlueprintPure, Category = "Ability")
    float GetCooldownTimeRemaining() const;

    /**
     * 获取技能等级
     * @return 当前技能等级
     */
    UFUNCTION(BlueprintPure, Category = "Ability")
    int32 GetAbilityLevel() const;

    /**
     * 检查是否可以升级技能
     * @return 是否可以升级
     */
    UFUNCTION(BlueprintPure, Category = "Ability")
    bool CanUpgradeAbility() const;

    /**
     * 升级技能
     * @return 是否升级成功
     */
    UFUNCTION(BlueprintCallable, Category = "Ability")
    bool UpgradeAbility();

protected:
    /** 技能图标 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    TObjectPtr<UTexture2D> AbilityIcon;

    /** 技能名称 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    FText AbilityName;

    /** 技能描述 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    FText AbilityDescription;

    /** 最大等级 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    int32 MaxLevel = 5;
};
```

## 🛍️ 库存系统

### UInventoryComponent

库存组件，管理角色的物品。

```cpp
class CRUNCH_API UInventoryComponent : public UActorComponent
{
public:
    /**
     * 添加物品到库存
     * @param Item 要添加的物品
     * @param Quantity 数量
     * @return 是否添加成功
     */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(const UPDA_ShopItem* Item, int32 Quantity = 1);

    /**
     * 从库存移除物品
     * @param Item 要移除的物品
     * @param Quantity 数量
     * @return 实际移除的数量
     */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 RemoveItem(const UPDA_ShopItem* Item, int32 Quantity = 1);

    /**
     * 获取物品数量
     * @param Item 要查询的物品
     * @return 物品数量
     */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetItemCount(const UPDA_ShopItem* Item) const;

    /**
     * 检查是否有足够的物品
     * @param Item 要检查的物品
     * @param Quantity 需要的数量
     * @return 是否有足够数量
     */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasEnoughItems(const UPDA_ShopItem* Item, int32 Quantity) const;

    /**
     * 获取所有物品
     * @return 物品数组
     */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    TArray<const UPDA_ShopItem*> GetAllItems() const;

    /**
     * 清空库存
     */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ClearInventory();

    /**
     * 库存变化委托
     */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryChanged, 
        const UPDA_ShopItem*, Item, int32, NewQuantity, int32, QuantityDelta);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

protected:
    /** 库存数据 */
    UPROPERTY(Replicated, SaveGame)
    TMap<const UPDA_ShopItem*, int32> InventoryItems;

    /** 最大库存容量 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 MaxInventorySize = 100;
};
```

### UPDA_ShopItem

商店物品数据资产。

```cpp
UCLASS(BlueprintType)
class CRUNCH_API UPDA_ShopItem : public UPrimaryDataAsset, public ITreeNodeInterface
{
public:
    /** 物品名称 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemName;

    /** 物品描述 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText ItemDescription;

    /** 物品图标 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<UTexture2D> ItemIcon;

    /** 物品价格 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    float ItemPrice = 0.0f;

    /** 合成所需的子物品 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    TArray<const UPDA_ShopItem*> RequiredItems;

    /** 使用此物品可以合成的物品 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
    TArray<const UPDA_ShopItem*> CraftableItems;

    // ITreeNodeInterface 实现
    virtual TArray<const ITreeNodeInterface*> GetParentNodes() const override;
    virtual TArray<const ITreeNodeInterface*> GetChildNodes() const override;
    virtual FText GetNodeDisplayName() const override { return ItemName; }
    virtual UTexture2D* GetNodeIcon() const override { return ItemIcon; }

    /**
     * 计算考虑背包物品后的实际价格
     * @param InventoryComponent 玩家库存
     * @return 实际需要支付的价格
     */
    UFUNCTION(BlueprintPure, Category = "Item")
    float CalculateActualPrice(const UInventoryComponent* InventoryComponent) const;

    /**
     * 检查是否可以合成
     * @param InventoryComponent 玩家库存
     * @return 是否可以合成
     */
    UFUNCTION(BlueprintPure, Category = "Item")
    bool CanCraft(const UInventoryComponent* InventoryComponent) const;
};
```

## 🎨 UI系统

### UUserWidget 扩展

#### UShopWidget

商店界面控件。

```cpp
class CRUNCH_API UShopWidget : public UUserWidget
{
public:
    /**
     * 显示物品的合成树
     * @param Item 要显示的物品
     */
    UFUNCTION(BlueprintCallable, Category = "Shop")
    void ShowItemCombinationTree(const UPDA_ShopItem* Item);

    /**
     * 购买物品
     * @param Item 要购买的物品
     * @return 是否购买成功
     */
    UFUNCTION(BlueprintCallable, Category = "Shop")
    bool PurchaseItem(const UPDA_ShopItem* Item);

    /**
     * 计算物品的实际价格
     * @param Item 物品
     * @return 实际价格
     */
    UFUNCTION(BlueprintPure, Category = "Shop")
    float GetItemActualPrice(const UPDA_ShopItem* Item) const;

protected:
    /** 商店物品列表 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTileView> ShopItemList;

    /** 物品合成树 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UItemTreeWidget> CombinationTree;

    /** 价格显示文本 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PriceDisplayText;

    /** 购买按钮 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> PurchaseButton;
};
```

#### UAbilityGauge

技能槽UI控件。

```cpp
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
public:
    /**
     * 配置技能显示数据
     * @param WidgetData 技能UI数据
     */
    UFUNCTION(BlueprintCallable, Category = "Ability")
    void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);

    /**
     * 更新冷却显示
     * @param CooldownPercent 冷却百分比 (0-1)
     */
    UFUNCTION(BlueprintCallable, Category = "Ability")
    void UpdateCooldown(float CooldownPercent);

    /**
     * 更新技能等级显示
     * @param Level 当前等级
     * @param MaxLevel 最大等级
     */
    UFUNCTION(BlueprintCallable, Category = "Ability")
    void UpdateAbilityLevel(int32 Level, int32 MaxLevel);

    /**
     * 设置是否可以释放技能
     * @param bCanCast 是否可以释放
     */
    UFUNCTION(BlueprintCallable, Category = "Ability")
    void SetCanCast(bool bCanCast);

protected:
    /** 技能图标 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> AbilityIcon;

    /** 技能等级文本 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> LevelText;

    /** 冷却遮罩 */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CooldownOverlay;
};
```

## 🌐 网络API

### FPlayerSelection

玩家选择数据结构。

```cpp
USTRUCT(BlueprintType)
struct CRUNCH_API FPlayerSelection
{
    GENERATED_BODY()

    /** 玩家ID */
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    FString PlayerID;

    /** 玩家昵称 */
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    FString PlayerNickname;

    /** 选择的角色 */
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    const UPDA_CharacterDefinition* SelectedCharacter = nullptr;

    /** 队伍ID */
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    uint8 TeamID = 0;

    /** 是否准备就绪 */
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    bool bIsReady = false;

    /** 玩家槽位ID */
    UPROPERTY(BlueprintReadWrite, Category = "Player")
    uint8 SlotID = 0;

    FPlayerSelection() = default;

    FPlayerSelection(const FString& InPlayerID, const FString& InNickname)
        : PlayerID(InPlayerID), PlayerNickname(InNickname) {}
};
```

### 网络RPC

```cpp
// 服务器端RPC - 可靠传输
UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
void Server_SelectCharacter(const UPDA_CharacterDefinition* Character);
bool Server_SelectCharacter_Validate(const UPDA_CharacterDefinition* Character);
void Server_SelectCharacter_Implementation(const UPDA_CharacterDefinition* Character);

// 客户端RPC - 可靠传输
UFUNCTION(Client, Reliable, BlueprintCallable)
void Client_OnCharacterSelected(const FPlayerSelection& PlayerSelection);
void Client_OnCharacterSelected_Implementation(const FPlayerSelection& PlayerSelection);

// 多播RPC - 不可靠传输（用于频繁更新）
UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
void Multicast_UpdatePlayerLocation(const FVector& NewLocation);
void Multicast_UpdatePlayerLocation_Implementation(const FVector& NewLocation);
```

## 🎯 事件系统

### 委托定义

```cpp
// 单播委托
DECLARE_DELEGATE_OneParam(FOnItemSelected, const UPDA_ShopItem*);

// 多播委托
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float /*CurrentHealth*/, float /*MaxHealth*/);

// 动态委托（蓝图可用）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDied, ACrunchCharacter*, DeadCharacter);

// 事件委托（只有拥有者可以广播）
DECLARE_EVENT_TwoParams(ACrunchCharacter, FOnLevelUp, int32 /*NewLevel*/, int32 /*SkillPoints*/);
```

### 使用示例

```cpp
// 绑定委托
HealthComponent->OnHealthChanged.AddDynamic(this, &AMyActor::HandleHealthChanged);

// 绑定Lambda
InventoryComponent->OnItemAdded.AddLambda([this](const UPDA_ShopItem* Item, int32 Quantity)
{
    UE_LOG(LogTemp, Log, TEXT("Added %d of %s"), Quantity, *Item->ItemName.ToString());
});

// 广播事件
OnCharacterDied.Broadcast(this);
```

## 🔧 实用工具

### FAbilityWidgetData

技能UI数据结构。

```cpp
USTRUCT(BlueprintType)
struct CRUNCH_API FAbilityWidgetData
{
    GENERATED_BODY()

    /** 技能图标 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    TObjectPtr<UTexture2D> AbilityIcon = nullptr;

    /** 技能名称 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    FText AbilityName;

    /** 当前等级 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    int32 CurrentLevel = 0;

    /** 最大等级 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    int32 MaxLevel = 5;

    /** 冷却剩余时间 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    float CooldownRemaining = 0.0f;

    /** 总冷却时间 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    float TotalCooldownTime = 0.0f;

    /** 是否可以释放 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    bool bCanCast = false;

    /** 是否有升级点可用 */
    UPROPERTY(BlueprintReadWrite, Category = "Ability")
    bool bUpgradeAvailable = false;
};
```

这个API文档涵盖了Crunch项目中最重要的类和接口，为开发者提供了详细的函数签名和使用说明。