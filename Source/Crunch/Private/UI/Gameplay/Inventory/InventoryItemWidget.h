// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "UI/Common/Items/ItemWidget.h"
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
	void SetSlotNumber(int32 NewSlotNumber);
	// 更新UI显示指定物品
	void UpdateInventoryItem(const UInventoryItem* Item);
	// 清空槽位
	void EmptySlot();
	// 获取槽位编号
	FORCEINLINE int32 GetSlotNumber() const { return SlotNumber; }
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

	// TODO:蓝耗我估计会删除，因为这个操作很唐（改成物品插槽位置+1或许不错呢）
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


	/******************************************/
	/*           GAS 相关功能                 */
	/******************************************/
public:
	// 开始冷却显示
	void StartCooldown(float CooldownDuration, float TimeRemaining);

private:
	// 冷却更新间隔（秒）
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;

	// // TODO: 移除魔法绑定的时候我就删了他
	// // 绑定施法状态变更委托
	// void BindCanCastAbilityDelegate();
	//
	// // 解绑施法状态变更委托
	// void UnBindCanCastAbilityDelegate();
	
	// 冷却结束处理
	void CooldownFinished();
	
	// 更新冷却显示
	void UpdateCooldown();
	
	// 清除冷却显示
	void ClearCooldown();

	// 冷却结束计时器句柄
	FTimerHandle CooldownDurationTimerHandle;
	
	// 冷却更新计时器句柄
	FTimerHandle CooldownUpdateTimerHandle;

	// 当前剩余冷却时间
	float CooldownTimeRemaining = 0.f;
	
	// 当前总冷却时间
	float CooldownTimeDuration = 0.f;

	// 动态材质参数：冷却百分比
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CooldownAmtDynamicMaterialParamName = "Percent";
	
	// 动态材质参数：图标纹理
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName IconTextureDynamicMaterialParamName = "Icon";
	
	// 动态材质参数：可施法状态
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CanCastDynamicMaterialParamName = "CanCast";

	// 设置图标纹理（重写基类方法）
	virtual void SetIcon(UTexture2D* IconTexture) override;
	
	// 冷却时间显示格式选项
	FNumberFormattingOptions CooldownDisplayFormattingOptions;
};
