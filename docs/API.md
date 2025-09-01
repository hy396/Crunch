# Crunch API 文档

## 📋 概览

这个文档描述了Crunch游戏项目中主要类和接口的API。所有API都是基于Unreal Engine 5的框架构建的。

## 🎮 核心游戏框架

### UMGameInstance

游戏实例类，管理游戏全局状态和在线服务。

```cpp
class CRUNCH_API UMGameInstance : public UGameInstance
{
public:
    /**
     * 游戏实例初始化
     */
    virtual void Init() override;

    /**
     * 开始匹配（服务器端调用）
     */
    void StartMatch();

    // === 登录功能 ===
    
    /**
     * 检查是否已登录
     */
    bool IsLoggedIn() const;
    
    /**
     * 检查是否正在登录中
     */
    bool IsLoggingIn() const;
    
    /**
     * 客户端通过账户门户登录
     */
    void ClientAccountPortalLogin();
    
    /**
     * 登录完成委托
     */
    FOnLoginCompleted OnLoginCompleted;

    // === 会话创建和搜索 ===
    
    /**
     * 请求创建并加入新会话
     * @param NewSessionName 会话名称
     */
    void RequestCreateAndJoinSession(const FName& NewSessionName);
    
    /**
     * 取消会话创建
     */
    void CancelSessionCreation();

    /**
     * 开始全局会话搜索
     */
    void StartGlobalSessionSearch();
    
    /**
     * 通过会话 ID 加入会话
     * @param SessionIdStr 会话 ID 字符串
     * @return 是否成功发起加入请求
     */
    bool JoinSessionWithId(const FString& SessionIdStr);
    
    /**
     * 加入会话失败委托
     */
    FOnJoinSesisonFailed OnJoinSessionFailed;
    
    /**
     * 全局会话搜索完成委托
     */
    FOnGlobalSessionSearchCompleted OnGlobalSessionSearchCompleted;

    // === 会话服务器功能 ===
    
    /**
     * 玩家加入会话（服务器端调用）
     * @param UniqueId 玩家唯一 ID
     */
    void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
    
    /**
     * 玩家离开会话（服务器端调用）
     * @param UniqueId 玩家唯一 ID
     */
    void PlayerLeft(const FUniqueNetIdRepl& UniqueId);

private:
    /**
     * 客户端登录实现
     * @param Type 登录类型
     * @param Id 用户 ID
     * @param Token 登录令牌
     */
    void ClientLogin(const FString& Type, const FString& Id, const FString& Token);
    
    /**
     * 加载关卡并监听连接
     * @param Level 关卡引用
     */
    void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};
```

### ACPlayerController

玩家控制器，处理玩家输入和网络通信。

```cpp
class CRUNCH_API ACPlayerController : public APlayerController, public IGenericTeamAgentInterface, public IChatInterface
{
public:
    /**
     * 设置团队ID
     */
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

    /**
     * 获取团队ID
     */
    virtual FGenericTeamId GetGenericTeamId() const override;

    /**
     * 比赛结束处理
     * @param ViewTarget 视角目标
     * @param WiningTeam 获胜队伍
     */
    void MatchFinished(AActor* ViewTarget, int WiningTeam);

    /**
     * 显示伤害数值
     * @param DamageAmount 伤害数值
     * @param TargetCharacter 目标角色
     * @param bCriticalHit 是否暴击
     * @param DamageType 伤害类型
     */
    UFUNCTION(Client, Reliable)
    void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit, FGameplayTag DamageType);

    /**
     * 服务器RPC：发送聊天消息
     * @param Message 消息内容
     * @param ChannelType 频道类型
     */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);

    /**
     * 客户端RPC：接收聊天消息
     * @param Message 聊天消息
     */
    UFUNCTION(Client, Reliable)
    void Client_ReceiveChatMessage(const FChatMessage& Message);

    // IChatInterface 接口实现
    virtual void SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType) override;
    virtual void ReceiveChatMessageFromServer(const FChatMessage& Message) override;

private:
    /**
     * 客户端比赛结束处理
     */
    UFUNCTION(Client, Reliable)
    void Client_MatchFinished(AActor* ViewTarget, int WiningTeam);

    // 玩家角色指针
    UPROPERTY()
    TObjectPtr<ACPlayerCharacter> CPlayerCharacter;

    // 团队ID
    UPROPERTY(Replicated)
    FGenericTeamId TeamID;

    // UI输入映射
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> UIInputMapping;
};
```

## 🧙‍♂️ 角色系统

### UPDA_CharacterDefinition

角色定义数据资产，包含角色的所有配置信息。

```cpp
class CRUNCH_API UPDA_CharacterDefinition : public UPrimaryDataAsset
{
public:
    /**
     * 获取当前数据资产的唯一标识符
     */
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
    
    /**
     * 获取角色定义资产类型（用于资产管理）
     */
    static FPrimaryAssetType GetCharacterDefinitionAssetType();

    /**
     * 获取角色显示名称
     */
    FString GetCharacterDisplayName() const;
    
    /**
     * 加载角色图标纹理
     */
    UTexture2D* LoadIcon() const;
    
    /**
     * 加载角色蓝图类
     */
    TSubclassOf<ACCharacter> LoadCharacterClass() const;
    
    /**
     * 加载显示用的动画蓝图
     */
    TSubclassOf<UAnimInstance> LoadDisplayAnimationBP() const;
    
    /**
     * 加载显示用的骨骼网格
     */
    USkeletalMesh* LoadDisplayMesh() const;
    
    /**
     * 获取能力映射表（输入ID到技能类的映射）
     */
    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* GetAbilities() const;

private:
    /** 角色显示名称 */
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    FString CharacterName;

    /** 角色图标纹理 */
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSoftObjectPtr<UTexture2D> CharacterIcon;

    /** 角色蓝图类 */
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSoftClassPtr<ACCharacter> CharacterClass;

    /** 显示用的动画蓝图 */
    UPROPERTY(EditDefaultsOnly, Category = "Character")
    TSoftClassPtr<UAnimInstance> DisplayAnimBP;
};
```

### ACCharacter

游戏角色基类。

```cpp
class ACCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface, public IRenderActorTargetInterface
{
public:
    ACCharacter();
    
    /**
     * 服务器中初始化
     */
    void ServerSideInit();
    
    /**
     * 客户端中初始化
     */
    void ClientSideInit();
    
    /**
     * 判断该角色是否由本地玩家控制
     */
    bool IsLocallyControlledByPlayer() const;

    /**
     * 获取技能表
     */
    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
    
    /**
     * 头顶UI颜色设置
     */
    void SetOverHeadWidgetColor();

    /**
     * 获取头像截取的位置
     */
    virtual FVector GetCaptureLocalPosition() const override;
    
    /**
     * 获取头像截取的旋转
     */
    virtual FRotator GetCaptureLocalRotation() const override;

    // === GAS相关 ===
    
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    
    /**
     * 在服务器端向自身发送游戏事件
     */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
    
    /**
     * 获取聚焦模式
     */
    bool GetIsInFocusMode() const;

    // === 死亡和复活 ===
    
    /**
     * 检查角色当前是否处于死亡状态
     */
    bool IsDead() const;

    /**
     * 立即执行角色重生操作
     */
    void RespawnImmediately();

    // === 团队 ===
    
    /**
     * 设置团队ID
     */
    virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

    /**
     * 获取团队ID
     */
    virtual FGenericTeamId GetGenericTeamId() const override;

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;

private:
    /** 技能系统组件 */
    UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
    TObjectPtr<UCAbilitySystemComponent> CAbilitySystemComponent;
    
    UPROPERTY()
    TObjectPtr<UCAttributeSet> CAttributeSet;
    
    /** 头顶UI */
    UPROPERTY(VisibleDefaultsOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> OverHeadWidgetComponent;
    
    /** 团队ID */
    UPROPERTY(ReplicatedUsing = OnRep_TeamID)
    FGenericTeamId TeamID;

    /** AI感知刺激源组件 */
    UPROPERTY()
    TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimuliSourceComponent;
};
};
```

## ⚔️ 技能系统 (GAS)

### UCAttributeSet

角色属性集，定义所有游戏属性。

```cpp
class UCAttributeSet : public UAttributeSet
{
public:
    // === 生命周期 ===
    
    /**
     * 用于声明哪些属性需要在网络中进行复制
     */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    /**
     * 当Attribute的CurrentValue被改变之前调用
     */
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    
    /**
     * 仅在instant Gameplay Effect使Attribute的BaseValue改变时触发
     */
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    // === 属性管理 ===
    
    /**
     * 根据缓存的生命百分比和新最大生命值重新计算生命值
     */
    void RescaleHealth();
    
    /**
     * 根据缓存的法力百分比和最大法力值重新计算法力值
     */
    void RescaleMana();

    // === 基础属性 ===
    
    /** 生命值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Health)

    /** 最大生命值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth)

    /** 法力值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana)

    /** 最大法力值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana)

    // === 伤害属性 ===
    
    /** 物理伤害 */
    UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
    FGameplayAttributeData AttackDamage;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackDamage)

    /** 法术伤害 */
    UPROPERTY(ReplicatedUsing = OnRep_MagicDamage)
    FGameplayAttributeData MagicDamage;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MagicDamage)

    /** 真实伤害 */
    UPROPERTY(ReplicatedUsing = OnRep_TrueDamage)
    FGameplayAttributeData TrueDamage;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, TrueDamage)

    /** 攻击力（物理攻击强度） */
    UPROPERTY(ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackPower)
    
    /** 法术强度（魔法攻击强度） */
    UPROPERTY(ReplicatedUsing = OnRep_MagicPower)
    FGameplayAttributeData MagicPower;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MagicPower)

    // === 防御属性 ===
    
    /** 护甲值 */
    UPROPERTY(ReplicatedUsing = OnRep_Armor)
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, Armor)

    /** 法术抗性（减少受到的魔法伤害） */
    UPROPERTY(ReplicatedUsing = OnRep_MagicResistance)
    FGameplayAttributeData MagicResistance;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MagicResistance)

    // === 移动属性 ===
    
    /** 移动速度 */
    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveSpeed)

    /** 移动加速度 */
    UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
    FGameplayAttributeData MoveAcceleration;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveAcceleration)

    // === 缓存属性 ===
    
    /** 缓存的生命百分比 */
    UPROPERTY()
    FGameplayAttributeData CachedHealthPercent;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, CachedHealthPercent)

    /** 缓存的法力百分比 */
    UPROPERTY()
    FGameplayAttributeData CachedManaPercent;
    ATTRIBUTE_ACCESSORS(UCAttributeSet, CachedManaPercent)

private:
    /**
     * 设置效果属性
     */
    void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
    
    /**
     * 伤害处理函数
     */
    void Damage(const FEffectProperties& Props, FGameplayTag DamageType, const float Damage);
    
    /**
     * 显示浮动数字
     */
    static void ShowFloatingText(const FEffectProperties& Props, float Damage, bool IsCriticalHitE, FGameplayTag DamageType);
    
    /**
     * 用于激活角色死亡被动的函数
     */
    void OnDeadAbility(const FEffectProperties& Props);

    // 属性复制回调函数
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION()
    void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);
    UFUNCTION()
    void OnRep_MagicDamage(const FGameplayAttributeData& OldMagicDamage);
    UFUNCTION()
    void OnRep_TrueDamage(const FGameplayAttributeData& OldTrueDamage);
    UFUNCTION()
    void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
    UFUNCTION()
    void OnRep_MagicPower(const FGameplayAttributeData& OldMagicPower);
    UFUNCTION()
    void OnRep_Armor(const FGameplayAttributeData& OldArmor);
    UFUNCTION()
    void OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance);
    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
    UFUNCTION()
    void OnRep_MoveAcceleration(const FGameplayAttributeData& OldMoveAcceleration);
};
```

### FAbilityWidgetData

技能控件数据结构，用于配置技能UI显示内容。

```cpp
USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
    GENERATED_BODY()

    /** 技能类 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayAbility> AbilityClass;

    /** 技能名称 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AbilityName;

    /** 技能图标 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> Icon;

    /** 技能描述 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;
};

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

库存管理组件，负责处理物品的添加、移除、使用和交易逻辑。

```cpp
class CRUNCH_API UInventoryComponent : public UActorComponent
{
public:
    UInventoryComponent();

    // === 事件委托 ===
    
    /** 物品添加事件委托 */
    FOnItemAddedDelegate OnItemAdded;
    
    /** 物品移除事件委托 */
    FOnItemRemovedDelegate OnItemRemoved;
    
    /** 物品堆叠数量变化事件委托 */
    FOnItemStackCountChangeDelegate OnItemStackCountChanged;

    /** 物品能力使用事件委托 */
    FOnItemAbilityCommitted OnItemAbilityCommitted;
    
    // === 物品操作 ===
    
    /**
     * 尝试激活指定句柄对应的物品
     */
    void TryActivateItem(const FInventoryItemHandle& ItemHandle);
    
    /**
     * 尝试购买商店物品
     */
    void TryPurchase(const UPDA_ShopItem* ItemToPurchase);
    
    /**
     * 出售指定物品
     */
    void SellItem(const FInventoryItemHandle& ItemHandle);
    
    /**
     * 尝试激活指定槽位的物品
     */
    void TryActivateItemInSlot(int32 SlotNumber);
    
    // === 查询方法 ===
    
    /**
     * 获取当前金币数量
     */
    float GetGold() const;
    
    /**
     * 获取库存容量
     */
    int32 GetCapacity() const;
    
    /**
     * 通过句柄获取库存物品
     */
    UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

    /**
     * 检查指定物品类型是否已达库存上限
     */
    bool IsFullFor(const UPDA_ShopItem* Item) const;

    /**
     * 检查所有槽位是否已被占用
     */
    bool IsAllSlotOccupied() const;
    
    /**
     * 获取指定物品可堆叠的库存实例
     */
    UInventoryItem* GetAvailableStackForItem(const UPDA_ShopItem* Item) const;
    
    /**
     * 尝试获取与商店物品对应的库存物品
     */
    UInventoryItem* TryGetItemForShopItem(const UPDA_ShopItem* Item) const;
    
    /**
     * 尝试获取与商店物品对应的库存物品句柄数组
     */
    TArray<FInventoryItemHandle> TryGetItemForShopItemHandles(const UPDA_ShopItem* Item) const;
    
    // === 价格计算 ===
    
    /**
     * 计算购买物物品所需的实际金币
     * @param Item 需要购买的物品
     * @param OutItemHandles 参与合成的物品(后续用来出售)
     * @return 事件购买金额
     */
    float GetPurchasePrice(const UPDA_ShopItem* Item, TArray<FInventoryItemHandle>& OutItemHandles) const;

    /**
     * 寻找可以参与合成的物品
     */
    void FindCombinationForItem(const UPDA_ShopItem* Item, TArray<FInventoryItemHandle>& OutItemHandles) const;
    
    /**
     * 查找合成指定物品所需的材料
     * @param Item 目标物品
     * @param OutIngredients 输出找到的材料
     * @param IngredientToIgnore 需要忽略的材料列表
     * @return 是否找到全部材料
     */
    bool FindIngredientForItem(const UPDA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients, const TArray<const UPDA_ShopItem*>& IngredientToIgnore = TArray<const UPDA_ShopItem*>{});
    
    // === 槽位管理 ===
    
    /**
     * 处理物品槽位变更
     */
    void ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlotNumber);

protected:
    virtual void BeginPlay() override;
    
private:
    /** 库存容量（槽位数） */
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    int32 Capacity = 6;

    /** 所有者的能力系统组件 */
    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

    /** 库存物品映射（句柄->物品实例） */
    UPROPERTY()
    TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

    // === 服务器 RPC ===
    
    /** 服务器端：处理购买请求 */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_Purchase(const UPDA_ShopItem* ItemToPurchase);

    /** 服务器端：处理购买请求，顺便卖出处理 */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_PurchaseItem(const UPDA_ShopItem* ItemToPurchase, const TArray<FInventoryItemHandle>& RemoveHandles);

    /** 服务器端：处理物品激活请求 */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ActivateItem(FInventoryItemHandle ItemHandle);

    /** 服务器端：处理物品出售请求 */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SellItem(FInventoryItemHandle ItemHandle);
    
    // === 客户端 RPC ===
    
    /** 客户端：处理物品添加通知 */
    UFUNCTION(Client, Reliable)
    void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPDA_ShopItem* Item, FGameplayAbilitySpecHandle GrantedAbilitySpecHandle);

    /** 客户端：处理物品移除通知 */
    UFUNCTION(Client, Reliable)
    void Client_ItemRemoved(FInventoryItemHandle ItemHandle);
    
    /** 客户端：处理物品堆叠数量变更通知 */
    UFUNCTION(Client, Reliable)
    void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount);
    
    // === 内部方法 ===
    
    /** 向库存添加新物品 */
    void GrantItem(const UPDA_ShopItem* NewItem);
    
    /** 全新购买物品逻辑 */
    void GrantItem(const UPDA_ShopItem* NewItem, float PurchasePrice, TArray<FInventoryItemHandle> RemoveHandles);

    /** 消耗物品（减少堆叠或移除） */
    void ConsumeItem(UInventoryItem* Item);
    
    /** 从库存完全移除物品 */
    void RemoveItem(UInventoryItem* Item);

    /** 尝试物品合成 */
    bool TryItemCombination(const UPDA_ShopItem* NewItem);
    
    /** 能力使用回调 */
    void AbilityCommitted(UGameplayAbility* CommittedAbility);
};
```

### UPDA_ShopItem

商店物品基础数据资产类，定义可在商店中交易的物品属性。

```cpp
class CRUNCH_API UPDA_ShopItem : public UPrimaryDataAsset
{
public:
    /**
     * 获取物品 ID
     */
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;

    /**
     * 获取商店物品的资产类型标识符
     */
    static FPrimaryAssetType GetShopItemAssetType();

    /**
     * 获取物品图标
     */
    UTexture2D* GetIcon() const;

    /**
     * 获取物品的名称
     */
    FText GetItemName() const;

    /**
     * 获取物品的描述
     */
    FText GetItemDescription() const;

    /**
     * 获取物品的购买价格
     */
    float GetPrice() const;

    /**
     * 获取物品的出售价格（设为购入价格的一半）
     */
    float GetSellPrice() const;
    
    /**
     * 获取装备时触发的 GE
     */
    TSubclassOf<UGameplayEffect> GetEquippedEffect() const;
    
    /**
     * 获取使用时触发的 GE
     */
    TSubclassOf<UGameplayEffect> GetConsumeEffect() const;
    
    /**
     * 获取物品授予的 GA
     */
    TSubclassOf<UGameplayAbility> GetGrantedAbility() const;

    /**
     * 获取物品授予的 GA 的默认对象
     */
    UGameplayAbility* GetGrantedAbilityCDO() const;

    /**
     * 检查物品是否可堆叠
     */
    bool GetIsStackable() const;

    /**
     * 检查物品是否可消耗
     */
    bool GetIsConsumable() const;

    /**
     * 获取最大堆叠数量
     */
    int32 GetMaxStackCount() const;

    /**
     * 获取合成所需的材料物品列表
     */
    const TArray<TSoftObjectPtr<UPDA_ShopItem>>& GetIngredients() const;

private:
    /** 物品图标资源引用 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSoftObjectPtr<UTexture2D> Icon;

    /** 物品基础购买价格 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    float Price;

    /** 物品显示名称 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    FText ItemName;

    /** 物品详细描述 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    FText ItemDescription;

    /** 标识物品是否为消耗品 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    bool bIsConsumable;

    /** 装备时应用的 GameplayEffect */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSubclassOf<UGameplayEffect> EquippedEffect;

    /** 使用时应用的 GameplayEffect */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSubclassOf<UGameplayEffect> ConsumeEffect;

    /** 物品授予的 GameplayAbility */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TSubclassOf<UGameplayAbility> GrantedAbility;

    /** 标识物品是否可堆叠 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    bool bIsStackable = false;

    /** 最大堆叠数量（仅在可堆叠时有效） */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    int MaxStackCount = 5;

    /** 合成/制作所需的材料物品列表 */
    UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
    TArray<TSoftObjectPtr<UPDA_ShopItem>> IngredientItems;
};
```

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

商店界面主控件，管理商店物品展示和购买。

```cpp
class CRUNCH_API UShopWidget : public UUserWidget
{
public:
    virtual void NativeConstruct() override;

private:
    /**
     * 商店物品列表
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTileView> ShopItemList;

    /**
     * 物品合成树控件
     */
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UItemTreeWidget> CombinationTree;
    
    /**
     * 显示计算价值的文本控件
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PriceDisplayText;

    /**
     * 购买按钮
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BuyButton;
    
    /**
     * 库存组件：获取玩家的库存
     */
    UPROPERTY()
    TObjectPtr<UInventoryComponent> OwnerInventoryComponent;
    
    /**
     * 当前左键选择的商店物品
     */
    UPROPERTY()
    TObjectPtr<const UShopItemWidget> CurrentSelectedItem;
    
    /**
     * 商店物品到控件的映射表
     */
    UPROPERTY()
    TMap<const UPDA_ShopItem*, const UShopItemWidget*> ItemsMap;

    /**
     * 加载商店物品
     */
    void LoadShopItems();

    /**
     * 商店物品加载完成
     */
    void ShopItemLoadFinished();

    /**
     * 商店物品生成
     */
    void ShopItemWidgetGenerated(UUserWidget& NewWidget);
    
    /**
     * 显示指定物品的合成树
     */
    void ShowItemCombination(const UShopItemWidget* ItemWidget);
    
    /**
     * 选择某个商品
     */
    void SelectedShopItem(const UShopItemWidget* ItemWidget);
    
    /**
     * 按钮点击事件处理
     */
    UFUNCTION()
    void OnBuyButtonClicked();
    
    /**
     * 禁用按钮，将金额置为0
     */
    void SetButtonNoEnabledAndPriceTextZero(const FInventoryItemHandle& ItemHandle = FInventoryItemHandle::InvalidHandle());
};
```
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

## 💬 聊天系统

### FChatMessage

聊天消息数据结构，包含完整的消息信息。

```cpp
USTRUCT(BlueprintType)
struct CRUNCH_API FChatMessage
{
    GENERATED_BODY()

    /** 发送者名称 */
    UPROPERTY(BlueprintReadOnly)
    FString SenderName;

    /** 消息内容 */
    UPROPERTY(BlueprintReadOnly)
    FString MessageContent;

    /** 聊天频道类型 */
    UPROPERTY(BlueprintReadOnly)
    EChatChannelType ChannelType;

    /** 发送者团队ID */
    UPROPERTY(BlueprintReadOnly)
    FGenericTeamId SenderTeamId;

    /** 时间戳 */
    UPROPERTY(BlueprintReadOnly)
    FDateTime Timestamp;

    FChatMessage();
    FChatMessage(const FString& InSenderName, const FString& InMessage, 
                EChatChannelType InChannelType, FGenericTeamId InTeamId);
};
```

### EChatChannelType

聊天频道类型枚举。

```cpp
UENUM(BlueprintType)
enum class EChatChannelType : uint8
{
    Team    UMETA(DisplayName = "队友聊天"),
    All     UMETA(DisplayName = "全体聊天")
};
```

### UChatWidget

聊天界面主控件，管理聊天消息的显示和输入。

```cpp
class CRUNCH_API UChatWidget : public UUserWidget
{
public:
    /**
     * 显示聊天界面
     */
    UFUNCTION(BlueprintCallable)
    void ShowChatWidget();

    /**
     * 隐藏聊天界面
     */
    UFUNCTION(BlueprintCallable)
    void HideChatWidget();

    /**
     * 切换聊天界面显示状态
     */
    UFUNCTION(BlueprintCallable)
    void ToggleChatWidget();

    /**
     * 添加消息到聊天列表
     * @param Message 聊天消息
     * @param bIsSelf 是否为自己发送
     * @param bIsTeammate 是否为队友发送
     */
    UFUNCTION(BlueprintCallable)
    void AddMessageToChat(const FChatMessage& Message, bool bIsSelf = false, bool bIsTeammate = false);

    /**
     * 显示临时消息弹窗（10秒后自动隐藏）
     * @param Message 聊天消息
     * @param bIsSelf 是否为自己发送
     * @param bIsTeammate 是否为队友发送
     */
    UFUNCTION(BlueprintCallable)
    void ShowTemporaryMessage(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

    /**
     * 发送聊天消息
     */
    UFUNCTION(BlueprintCallable)
    void SendChatMessage();

    /**
     * 设置输入焦点到聊天输入框
     */
    UFUNCTION(BlueprintCallable)
    void FocusChatInput();

    /**
     * 检查是否处于临时消息模式
     * @return 是否在临时模式
     */
    UFUNCTION(BlueprintCallable)
    bool IsInTemporaryMode() const;

    /**
     * 设置发送消息后是否自动隐藏聊天框
     * @param bAutoHide 是否自动隐藏
     */
    void SetAutoHideAfterSend(bool bAutoHide);
};
```

### UChatMessageItemWidget

单条聊天消息显示控件，支持富文本格式和弹幕模式。

```cpp
class CRUNCH_API UChatMessageItemWidget : public UUserWidget
{
public:
    /**
     * 设置聊天消息数据
     * @param Message 聊天消息
     */
    UFUNCTION(BlueprintCallable)
    void SetChatMessage(const FChatMessage& Message);

    /**
     * 设置聊天消息数据并指定颜色类型
     * @param Message 聊天消息
     * @param bIsSelf 是否为自己发送
     * @param bIsTeammate 是否为队友发送
     */
    UFUNCTION(BlueprintCallable)
    void SetChatMessageWithColors(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate);

    /**
     * 设置为弹幕模式（使用弹幕动画）
     * @param Message 聊天消息
     * @param bIsSelf 是否为自己发送
     * @param bIsTeammate 是否为队友发送
     * @param BarragePanel 弹幕容器面板
     */
    UFUNCTION(BlueprintCallable)
    void SetAsBarrageMode(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate, UCanvasPanel* BarragePanel);

    /**
     * 停止弹幕动画
     */
    UFUNCTION(BlueprintCallable)
    void StopBarrageAnimation();

private:
    /**
     * 格式化带颜色的聊天消息文本
     * @param Message 聊天消息
     * @param bIsSelf 是否为自己发送
     * @param bIsTeammate 是否为队友发送
     * @return 格式化后的富文本字符串
     */
    FString FormatChatMessageWithColors(const FChatMessage& Message, bool bIsSelf, bool bIsTeammate) const;

    /**
     * 处理消息内容中的特殊字符转义
     * @param Content 原始内容
     * @return 转义后的安全内容
     */
    FString SanitizeMessageContent(const FString& Content) const;

    /**
     * 获取频道类型颜色标签
     * @param ChannelType 频道类型
     * @return 颜色标签字符串
     */
    FString GetChannelColorTag(EChatChannelType ChannelType) const;

    /**
     * 获取发送者类型颜色标签
     * @param bIsSelf 是否为自己发送
     * @param bIsTeammate 是否为队友发送
     * @return 颜色标签字符串
     */
    FString GetSenderTypeColorTag(bool bIsSelf, bool bIsTeammate) const;

    // 弹幕相关私有方法
    UFUNCTION()
    void UpdateBarragePosition();

    UFUNCTION()
    void OnBarrageFinished();
};
```

### IChatInterface

聊天接口，定义聊天消息收发的标准接口。

```cpp
class CRUNCH_API IChatInterface
{
public:
    /**
     * 发送聊天消息到服务器
     * @param Message 消息内容
     * @param ChannelType 频道类型
     */
    virtual void SendChatMessageToServer(const FString& Message, EChatChannelType ChannelType) = 0;

    /**
     * 接收来自服务器的聊天消息
     * @param Message 聊天消息
     */
    virtual void ReceiveChatMessageFromServer(const FChatMessage& Message) = 0;
};
```

### 网络RPC函数

聊天系统的网络同步通过以下RPC函数实现（在ACPlayerController和ALobbyPlayerController中）：

```cpp
// 客户端发送消息到服务器
UFUNCTION(Server, Reliable, WithValidation)
void Server_SendChatMessage(const FString& Message, EChatChannelType ChannelType);

// 服务器发送消息到客户端
UFUNCTION(Client, Reliable)
void Client_ReceiveChatMessage(const FChatMessage& Message);
```

## 🎯 事件系统

### 委托定义

``cpp
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

``cpp
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