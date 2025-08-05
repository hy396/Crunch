// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"
#include "InventoryWidget.generated.h"

class UInventoryContextMenuWidget;
/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 控件初始化
	virtual void NativeConstruct() override;
	
	// 焦点变化事件处理
	virtual void NativeOnFocusChanging(
		const FWeakWidgetPath& PreviousFocusPath, 
		const FWidgetPath& NewWidgetPath, 
		const FFocusEvent& InFocusEvent
	) override;
	
private:
	// 上下文菜单控件类(内含使用和售出两个按钮)
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuWidgetClass;

	// 当前显示的上下文菜单实例
	UPROPERTY()
	TObjectPtr<UInventoryContextMenuWidget> ContextMenuWidget;

	// 创建上下文菜单
	void SpawnContextMenu();
	
	// 出售当前焦点物品
	UFUNCTION()
	void SellFocusedItem();
	
	// 使用当前焦点物品
	UFUNCTION()
	void UseFocusedItem();

	// 设置上下文菜单可见性
	void SetContextMenuVisible(bool bContextMenuVisible);
	
	// 切换上下文菜单显示状态
	void ToggleContextMenu(const FInventoryItemHandle& ItemHandle);
	
	// 清除上下文菜单
	void ClearContextMenu();

	// 当前焦点物品句柄
	FInventoryItemHandle CurrentFocusedItemHandle;
	
	// UI绑定：物品列表容器（使用WrapBox自动布局）
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> ItemList;

	// 单个物品控件类
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

	// 关联的库存组件
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// 所有物品控件实例数组
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemWidget>> ItemWidgets;
	
	// 物品句柄到控件的映射
	UPROPERTY()
	TMap<FInventoryItemHandle, TObjectPtr<UInventoryItemWidget>> PopulatedItemEntryWidgets;

	// 处理物品添加事件
	void ItemAdded(const UInventoryItem* InventoryItem);
	
	// 处理物品堆叠数量变化事件
	void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

	// 获取下一个可用槽位控件
	UInventoryItemWidget* GetNextAvailableSlot() const;

	// 处理物品拖放事件
	void HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget);

	// 处理物品移除事件
	void ItemRemoved(const FInventoryItemHandle& ItemHandle);

	// 处理物品能力提交事件（冷却开始）
	void ItemAbilityCommitted(
		const FInventoryItemHandle& ItemHandle, 
		float CooldownDuration, 
		float CooldownTimeRemaining
	);
};
