// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "PDA_ShopItem.h"
#include "UObject/Object.h"
#include "InventoryItem.generated.h"


// 委托声明：当物品授予的能力可释放状态更新时广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityCanCastUpdatedDelegate, bool /*bCanCast*/)

/**
 * 库存物品的唯一句柄结构体，用于标识和管理库存中的物品实例
 */
USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	// 默认构造函数
	FInventoryItemHandle();
	
	// 获取无效句柄
	static FInventoryItemHandle InvalidHandle();
	
	// 创建新句柄
	static FInventoryItemHandle CreateHandle();

	// 检查句柄是否有效
	bool IsValid() const;
	
	// 获取句柄ID
	uint32 GetHandleId() const { return HandleId; }
	
private:
	// 使用指定ID构造句柄
	explicit FInventoryItemHandle(uint32 Id);

	// 句柄的唯一标识符
	UPROPERTY()
	uint32 HandleId;

	// 生成下一个句柄ID
	static uint32 GenerateNextId();
	
	// 获取无效ID值
	static uint32 GetInvalidId();
};

// 句柄相等运算符重载
bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);

// 句柄哈希函数重载
uint32 GetTypeHash(const FInventoryItemHandle& Key);

/**
 * 库存物品类，表示玩家库存中的一个实际物品实例
 */
UCLASS()
class CRUNCH_API UInventoryItem : public UObject
{
	GENERATED_BODY()
public:
	// 当物品授予的能力可释放状态变更时广播的委托
	FOnAbilityCanCastUpdatedDelegate OnAbilityCanCastUpdated;
	
	UInventoryItem();
	// 检查物品是否有效初始化
	bool IsValid() const;

	/**
	 * 尝试添加物品堆叠数量
	 * @return 是否成功增加堆叠数量
	 */
	bool AddStackCount();
	/**
	 * 尝试减少堆叠数量
	 * @return 减少后堆叠是否非空
	 */
	bool ReduceStackCount();
	/**
	 * 设置堆叠数量
	 * @param NewStackCount 设置的堆叠数量
	 * @return 是否成功设置
	 */
	bool SetStackCount(int32 NewStackCount);

	// 检查堆叠是否已满
	bool IsStackFull() const;

	// 检查此物品实例是否对应指定商店物品
	bool IsForItem(const UPDA_ShopItem* Item) const;
	
	// 检查此物品是否授予特定能力
	bool IsGrantingAbility(TSubclassOf<class UGameplayAbility> AbilityClass) const;
	
	// 检查此物品是否授予任何能力
	bool IsGrantingAnyAbility() const;
	
	/**
	 * 初始化物品实例
	 * @param NewHandle 分配给此物品的唯一句柄
	 * @param NewShopItem 关联的商店物品资产
	 * @param AbilitySystemComponent 拥有者的能力系统组件
	 */
	void InitItem(const FInventoryItemHandle& NewHandle, const UPDA_ShopItem* NewShopItem, UAbilitySystemComponent* AbilitySystemComponent);
	// 获取关联的商店物品资产
	const UPDA_ShopItem* GetShopItem() const { return ShopItem; }
	// 获取物品的唯一句柄
	FInventoryItemHandle GetHandle() const { return Handle; }

	// 尝试激活物品授予的能力
	bool TryActivateGrantedAbility();
	
	// 应用物品的消耗效果
	void ApplyConsumeEffect();
	
	// 移除所有应用的游戏能力系统修改
	void RemoveGASModifications();

	// 获取当前堆叠数量
	FORCEINLINE int32 GetStackCount() const { return StackCount; }
	
	// 设置物品在库存中的槽位
	void SetSlot(int32 NewSlot);
	
	// 获取物品在库存中的槽位
	int32 GetItemSlot() const { return Slot; }

	// 获取能力剩余冷却时间
	float GetAbilityCooldownTimeRemaining() const;
	
	// 获取能力总冷却时长
	float GetAbilityCooldownDuration() const;

	// TODO:没必要，等着我删吧
	// 获取能力法力消耗
	float GetAbilityManaCost() const;
	
	// 检查能力当前是否可施放
	bool CanCastAbility() const;
	
	// 获取授予的能力规格句柄
	FGameplayAbilitySpecHandle GetGrantedAbilitySpecHandle() const { return GrantedAbilitySpecHandle; }
	
	// 设置授予的能力规格句柄
	void SetGrantedAbilitySpecHandle(FGameplayAbilitySpecHandle SpecHandle) { GrantedAbilitySpecHandle = SpecHandle; }

private:
	// 应用游戏能力系统修改（效果和能力）
	void ApplyGASModifications();

	// 拥有者的能力系统组件
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	// TODO:坐等删除
	// 法力更新
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	
	// 关联的商店物品资产
	UPROPERTY()
	TObjectPtr<const UPDA_ShopItem> ShopItem;
	// 物品的唯一句柄
	FInventoryItemHandle Handle;

	// 当前堆叠数量
	int32 StackCount;
	
	// 在库存中的槽位索引
	int32 Slot;

	// 应用的装备效果句柄
	FActiveGameplayEffectHandle AppliedEquipedEffectHandle;
	
	// 授予的能力规格句柄
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
