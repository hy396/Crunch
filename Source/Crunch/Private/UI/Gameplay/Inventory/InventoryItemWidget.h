// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "UI/Common/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	// 检查槽位是否为空
	bool IsEmpty() const;
	// 设置槽位编号
	void SetSlotNumber(int NewSlotNumber);
	// 更新UI显示指定物品
	void UpdateInventoryItem(const UInventoryItem* Item);
	// 清空槽位
	void EmptySlot();
	// 获取槽位编号
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	// 更新堆叠数量显示
	void UpdateStackCount();
private:
	// 更新施法状态显示（是否可施放）
	void UpdateCanCastDisplay(bool bCanCast);
	
	// 空槽位时显示的默认纹理
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TObjectPtr<UTexture2D> EmptyTexture;
	// UI绑定：堆叠数量文本
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> StackCountText;

	// UI绑定：冷却倒计时文本
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownCountText;

	// UI绑定：冷却总时间文本
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;

	// UI绑定：法力消耗文本
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ManaCostText;
	
	// 当前显示的库存物品
	UPROPERTY()
	const UInventoryItem* InventoryItem;
	// 当前槽位编号
	int32 SlotNumber;
};
