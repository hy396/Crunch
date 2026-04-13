# API 参考

本文档列出 Crunch 项目中核心类的公开接口。详细的系统设计见各专题文档。

## 目录

- [角色系统](#角色系统)
- [技能系统](#技能系统)
- [装备系统](#装备系统)
- [框架](#框架)
- [网络](#网络)
- [玩家](#玩家)
- [AI](#ai)

---

## 角色系统

### ACCharacter

基础角色类，所有可战斗角色的父类。

**接口实现**: `IAbilitySystemInterface`, `IGenericTeamAgentInterface`, `IRenderActorTargetInterface`

**组件**:

- `UAbilitySystemComponent* AbilitySystemComponent`
- `UCAttributeSet* AttributeSet`
- `UOverHeadStatsGauge*` — 头顶状态条（血条/蓝条）

**关键方法**:

```cpp
// GAS
UAbilitySystemComponent* GetAbilitySystemComponent() const override;
void InitAbilitySystem();
void ApplyFullStatEffect();

// 队伍
void SetGenericTeamId(const FGenericTeamId& NewTeamId);
FGenericTeamId GetGenericTeamId() const override;
ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

// 战斗状态
void Die();
void Respawn();
void Stunned(UAnimMontage* StunMontage);  // 播放眩晕蒙太奇
bool IsDead() const;

// 头顶UI
void ShowOverHeadStats();
void HideOverHeadStats();
```

### ACPlayerCharacter

玩家角色，继承 ACCharacter。

**额外接口**: `ICombatInterface`

**额外组件**:

- `USpringArmComponent*` — 摄像机臂
- `UCameraComponent*` — 玩家摄像机
- `UMotionWarpingComponent*` — 动画位移
- `UInventoryComponent*` — 背包
- `UCHeroAttributeSet*` — 英雄属性集

**关键方法**:

```cpp
UInventoryComponent* GetInventoryComponent() const;
UCHeroAttributeSet* GetHeroAttributeSet() const;

// 输入（Enhanced Input System）
void AbilityInputPressed(ECAbilityInputID InputID);
void AbilityInputReleased(ECAbilityInputID InputID);
```

### UPDA_CharacterDefinition

角色数据资产（`UPrimaryDataAsset`），定义角色的外观和初始能力。

**属性**:

```cpp
USkeletalMesh* CharacterMesh;
TSubclassOf<UAnimInstance> AnimInstanceClass;
TMap<ECAbilityInputID, TSubclassOf<UCGameplayAbility>> AbilityTable;  // 技能映射表
TSubclassOf<UGameplayEffect> InitStatEffect;  // 初始属性效果
```

---

## 技能系统

详见 [GAS_SYSTEM.md](GAS_SYSTEM.md)。

### UCAbilitySystemComponent

```cpp
void InitAbilitiesFromCharacterDef(const UPDA_CharacterDefinition* Def);
void UpgradeAbility(ECAbilityInputID InputID);
void ApplyFullStatEffect();
```

### UCGameplayAbility

```cpp
// 伤害应用
void ApplyDamageToActor(AActor* Target, const FGenericDamageEffectDef& Damage, int Level);
void ApplyDamageToTargetDataHandle(
    const FGameplayAbilityTargetDataHandle& TargetData,
    const FGenericDamageEffectDef& Damage, int Level);

// 推力
void PushCharacterFromLocation(ACCharacter* Target, FVector Origin, float Force);
```

### UCAttributeSet

基础属性集，提供宏辅助的属性访问器：

```cpp
// 每个属性自动生成:
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
```

关键回调:

```cpp
virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
```

### UCAbilitySystemStatics

```cpp
static FGameplayTag GetCameraShakeGameplayCueTag();   // 缓存的镜头震动标签
static FGameplayTag GetDamageNumberGameplayCueTag();   // 缓存的伤害数字标签
```

---

## 装备系统

### UPDA_ShopItem

商店物品数据资产（`UPrimaryDataAsset`）。

```cpp
UTexture2D* GetIcon() const;
FText GetItemName() const;
FText GetItemDescription() const;
float GetPrice() const;
float GetSellPrice() const;                              // 购买价的 50%
TArray<TSubclassOf<UGameplayEffect>> GetEquippedEffects() const;  // 装备效果
TArray<TSubclassOf<UGameplayEffect>> GetConsumeEffects() const;   // 消耗效果
TSubclassOf<UGameplayAbility> GetGrantedAbility() const;          // 赋予的技能
bool GetIsStackable() const;
bool GetIsConsumable() const;
int32 GetMaxStackCount() const;
const TArray<TSoftObjectPtr<UPDA_ShopItem>>& GetIngredients() const;  // 合成材料
```

### UInventoryComponent

背包组件（`UActorComponent`），附加在玩家角色上。

**委托**:

```cpp
FOnItemAddedDelegate OnItemAdded;                // 物品添加
FOnItemRemovedDelegate OnItemRemoved;            // 物品移除
FOnItemStackCountChangeDelegate OnItemStackCountChanged;  // 堆叠变化
FOnItemAbilityCommitted OnItemAbilityCommitted;  // 物品技能使用（冷却开始）
```

**客户端方法**:

```cpp
void TryPurchase(const UPDA_ShopItem* Item);     // 购买（触发 Server RPC）
void SellItem(const FInventoryItemHandle& Handle);
void TryActivateItem(const FInventoryItemHandle& Handle);
void TryActivateItemInSlot(int32 SlotNumber);    // 按格子激活
void ItemSlotChanged(const FInventoryItemHandle& Handle, int32 NewSlot);  // 拖拽换位

float GetGold() const;
int32 GetCapacity() const;                       // 默认 6
bool IsAllSlotOccupied() const;
bool IsFullFor(const UPDA_ShopItem* Item) const;

// 合成查询
float GetPurchasePrice(const UPDA_ShopItem* Item, TArray<FInventoryItemHandle>& OutHandles) const;
void FindCombinationForItem(const UPDA_ShopItem* Item, TArray<FInventoryItemHandle>& OutHandles) const;
UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;
```

**Server RPC**:

```cpp
void Server_Purchase(const UPDA_ShopItem* Item);                    // 简单购买
void Server_PurchaseItem(const UPDA_ShopItem* Item,
    const TArray<FInventoryItemHandle>& RemoveHandles);             // 合成购买
void Server_ActivateItem(FInventoryItemHandle Handle);
void Server_SellItem(FInventoryItemHandle Handle);
```

**Client RPC**:

```cpp
void Client_ItemAdded(FInventoryItemHandle Handle, const UPDA_ShopItem* Item,
    FGameplayAbilitySpecHandle GrantedAbility);
void Client_ItemRemoved(FInventoryItemHandle Handle);
void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int32 NewCount);
```

### UInventoryItem

物品实例（`UObject`），由 InventoryComponent 内部管理。

```cpp
bool IsValid() const;
const UPDA_ShopItem* GetShopItem() const;
FInventoryItemHandle GetHandle() const;
int32 GetStackCount() const;
int32 GetItemSlot() const;
void SetSlot(int32 NewSlot);
bool AddStackCount();
bool ReduceStackCount();
bool IsStackFull() const;

// 技能相关
bool TryActivateGrantedAbility();
float GetAbilityCooldownTimeRemaining() const;
float GetAbilityCooldownDuration() const;
```

### FInventoryItemHandle

物品唯一标识，值类型。

```cpp
static FInventoryItemHandle CreateHandle();      // 生成新 ID
static FInventoryItemHandle InvalidHandle();
bool IsValid() const;
uint32 GetHandleId() const;
```

---

## 框架

### UMGameInstance

**登录**:

```cpp
void ClientAccountPortalLogin();
void ClientLogin(const FString& Type, const FString& Id, const FString& Token);
bool Logout();
void CancelLogin();
bool IsLoggedIn() const;
bool IsLoggingIn() const;

// 委托
FOnLoginCompleted OnLoginCompleted;  // (bool bSuccess, FString UserId, FString Error)
```

**会话管理**:

```cpp
void RequestCreateAndJoinSession(const FName& SessionName);
void CancelSessionCreation();
void StartGlobalSessionSearch();
void StopAllSessionFindings();
bool JoinSessionWithId(const FString& SessionIdStr);

// 委托
FOnGlobalSessionSearchCompleted OnGlobalSessionSearchCompleted;
FOnJoinSesisonFailed OnJoinSessionFailed;
```

**服务器端**:

```cpp
void StartMatch();
void CreateSession();
void TerminateSessionServer();
void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
void PlayerLeft(const FUniqueNetIdRepl& UniqueId);
void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
```

**关卡引用**:

```cpp
TSoftObjectPtr<UWorld> MainMenuLevel;
TSoftObjectPtr<UWorld> LobbyLevel;
TSoftObjectPtr<UWorld> GameLevel;
```

### ACGameMode

```cpp
void AddPlayerKillForTeam(const FGenericTeamId& TeamID);
FGenericTeamId GetTeamIDForPlayer(const AController* Controller) const;
AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const;
AStormCore* GetStormCore() const;
void MatchFinished(AActor* ViewTarget, int WiningTeam);
```

### ACGameState

**角色选择**:

```cpp
void RequestPlayerSelectionChange(const APlayerState* Player, uint8 DesiredSlot);
void SetCharacterSelected(const APlayerState* Player, const UPDA_CharacterDefinition* Def);
void SetCharacterDeselected(const UPDA_CharacterDefinition* Def);
bool IsSlotOccupied(uint8 SlotId) const;
bool IsDefinitionSelected(const UPDA_CharacterDefinition* Def) const;
const TArray<FPlayerSelection>& GetPlayerSelection() const;
bool CanStartMatch() const;

// 委托
FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;
```

**击杀追踪**:

```cpp
void AddTeamOnePlayerKillCount();
void AddTeamTwoPlayerKillCount();

UFUNCTION(NetMulticast, Reliable)
void Multicast_OnPlayerKilled(AMPlayerState* Killer, AMPlayerState* Victim,
    const TArray<AMPlayerState*>& Assists);

// 委托
FOnPlayerKilled OnPlayerKilled;
```

**Replicated 属性**:

```cpp
int32 TeamOnePlayerKillCount;
int32 TeamTwoPlayerKillCount;
TArray<FPlayerSelection> PlayerSelectionArray;
```

### AStormCore

```cpp
float GetProgress() const;

// 委托
FOnGoalReachedDelegate OnGoalReachedDelegate;                  // (AActor* Core, int WinTeam)
FonTeamInfluncerCountUpdatedDelegate OnTeamInfluenceCountUpdated;  // (int Team1, int Team2)
```

**配置**:

```cpp
float InfluenceRadius = 1000.f;
float MaxMoveSpeed = 500.f;
AActor* TeamOneGoal;
AActor* TeamTwoGoal;
AActor* TeamOneCore;
AActor* TeamTwoCore;
```

---

## 网络

### UTNetStatics

蓝图函数库，会话配置工具。

```cpp
static FOnlineSessionSettings GenerateOnlineSessionSettings(
    const FName& SessionName, const FString& SearchId, int32 Port);

static IOnlineSessionPtr GetSessionPtr();
static IOnlineIdentityPtr GetIdentityPtr();

static uint8 GetPlayerCountPerTeam();    // 默认 5
static bool IsSessionServer(const UObject* WorldContext);

// 命令行参数解析
static FString GetCommandlineArgAsString(const FName& ParamName);
static int GetCommandlineArgAsInt(const FName& ParamName);

// 配置键
static FString GetCoordinatorURL();
static FString GetDefaultCoordinatorURL();
static int GetSessionPort();
```

### ATGameSession

```cpp
virtual bool ProcessAutoLogin() override;
virtual void RegisterPlayer(APlayerController* NewPlayer,
    const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
virtual void UnregisterPlayer(FName FromSession, const FUniqueNetIdRepl& UniqueId) override;
```

---

## 玩家

### AMPlayerState

```cpp
UPDA_CharacterDefinition* SelectedDefinition;  // 选择的角色定义
uint8 SlotId;                                   // 槽位 ID
FGenericTeamId TeamId;                          // 根据槽位自动分配
```

### ACPlayerController

**UI 控制**:

```cpp
void ToggleShop();
void ToggleGameplayMenu();
void ToggleChat();
void ToggleAttributePanel();
```

**聊天** (实现 `IChatInterface`):

```cpp
void SendChatMessageToServer(const FString& Message, EChatChannelType Channel) override;
void ReceiveChatMessageFromServer(const FChatMessage& Message) override;

UFUNCTION(Server, Reliable, WithValidation)
void Server_SendChatMessage(const FString& Message, EChatChannelType Channel);

UFUNCTION(Client, Reliable)
void Client_ReceiveChatMessage(const FChatMessage& Message);
```

**战斗反馈**:

```cpp
void ShowDamageNumber(float Damage, ACharacter* Target);  // 伤害数字
void OnTeamKillCountUpdated(int32 Team1Kills, int32 Team2Kills);
```

---

## AI

### ACAIController

```cpp
// AI 感知
UAIPerceptionComponent* AIPerceptionComponent;

// 行为树
UBehaviorTree* BehaviorTreeAsset;
UBlackboardData* BlackboardAsset;

// 队伍
ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
```

### AMinionBarrack

```cpp
// 配置
float SpawnInterval;        // 生成间隔
int32 SpawnGroupSize;       // 每批数量
TSubclassOf<AMinion> MinionClass;

// 方法
void SpawnMinionGroup();
void SetGoal(AActor* GoalActor);
```

### UBTTask_SendInputToAbilitySystem

行为树任务节点，将 AI 决策转化为 GAS 输入：

```cpp
ECAbilityInputID AbilityInputID;  // 要触发的技能输入
```
