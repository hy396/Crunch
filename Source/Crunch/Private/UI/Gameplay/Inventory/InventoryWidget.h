// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"
#include "InventoryWidget.generated.h"

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
	// virtual void NativeOnFocusChanging(
	// 	const FWeakWidgetPath& PreviousFocusPath, 
	// 	const FWidgetPath& NewWidgetPath, 
	// 	const FFocusEvent& InFocusEvent
	// ) override;
	
private:
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
	// void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

	// 获取下一个可用槽位控件
	UInventoryItemWidget* GetNextAvailableSlot() const;
};
