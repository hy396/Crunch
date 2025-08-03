// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "UI/Common/ItemWidget.h"
#include "InventoryItemWidget.generated.h"


class UInventoryItemWidget;

// 定义委托：当库存物品被拖放时触发
DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnInventoryItemDropped, 
	UInventoryItemWidget* /* 目标槽位 */, 
	UInventoryItemWidget* /* 来源槽位 */
);

// 定义委托：当按钮点击时触发
DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnButtonClick, 
	const FInventoryItemHandle& /* 物品句柄 */
);

/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()
public:
	// 委托：当物品被拖放到此槽位时触发
	FOnInventoryItemDropped OnInventoryItemDropped;

	// 委托：当左键点击此物品时触发
	FOnButtonClick OnLeftButtonClicked;
	
	// 委托：当右键点击此物品时触发
	FOnButtonClick OnRightButtonClicked;
	
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
	// 获取图标
	UTexture2D* GetIconTexture();
	
	// 获取关联的库存物品
	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }
	
	// 获取物品句柄
	FInventoryItemHandle GetItemHandle() const;

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

	// 右键点击事件处理
	virtual void RightButtonClicked() override;
	
	// 左键点击事件处理
	virtual void LeftButtonClicked() override;
	
	/******************************************/
	/*           拖放功能                     */
	/******************************************/
private:
	// 检测到拖拽时触发
	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, 
		const FPointerEvent& InMouseEvent, 
		UDragDropOperation*& OutOperation
	) override;
	
	// 物品被拖放到此控件时触发（放置操作）
	virtual bool NativeOnDrop(
		const FGeometry& InGeometry, 
		const FDragDropEvent& InDragDropEvent, 
		UDragDropOperation* InOperation
	) override;

	// 拖放操作类
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	TSubclassOf<class UInventoryItemDragDropOp> DragDropOpClass;
};
