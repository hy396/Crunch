// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ShopItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/PDA_ShopItem.h"
#include "ShopWidget.generated.h"

class UItemTreeWidget;

/**
 * 商店界面主控件
 * 功能：
 *   - 显示可购买的商品列表
 *   - 展示物品的合成关系树
 *   - 管理商店物品加载和显示
 *   - 处理物品选择和合成展示
 */
UCLASS()
class CRUNCH_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	// 商店物品列表
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> ShopItemList;

	// 物品合成树控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UItemTreeWidget> CombinationTree;
	
	// 加载商店物品
	void LoadShopItems();

	// 商店物品加载完成
	void ShopItemLoadFinished();

	// 商店物品生成
	void ShopItemWidgetGenerated(UUserWidget& NewWidget);
	
	// 商店物品到控件的映射表
	// 用途：快速查找物品对应的控件实例
	UPROPERTY()
	TMap<const UPDA_ShopItem*, const UShopItemWidget*> ItemsMap;

	// 库存组件：获取玩家的库存
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwnerInventoryComponent;

	// 显示指定物品的合成树
	void ShowItemCombination(const UShopItemWidget* ItemWidget);

	/**
	 * TODO:添加一个用来存储当前左键选择的商店物品变量
	 * 添加一个计算当前物品的价值的显示文本UI
	 * 文本UI的价值计算方式：物品价格 - （背包中已经拥有的子节点物品）
	 * 因此这个文本UI还需要一个获取当前选择商店物品的子节点树的函数
	 * 实现原理：获取物品的 子节点树，若遇到了有背包中拥有的节点的物品，则减去物品价格，退出该节点的深度搜索
	 * 直到找到最底层的节点，也没有找到背包中有的物品，退出搜索
	 * 再创建一个按钮，该按钮的点击事件绑定购买功能，购买的时候扣除的钱就是改物品计算好的价格
	 * 购买的时候会去除背包中拥有的该节点的子节点的物品
	 * 
	 */

	/** 当前左键选择的商店物品 */
	UPROPERTY()
	TObjectPtr<const UShopItemWidget> CurrentSelectedItem;
	
	/** 显示计算价值的文本控件 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PriceDisplayText;

	/** 购买按钮 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BuyButton;

	// 选择某个商品
	void SelectedShopItem(const UShopItemWidget* ItemWidget);

	/**
	 * TODO:感觉可以把实际价格的重构修改到背包组件中去
	 * 这样可以在商店调用背包组件的函数，减少重复定义
	 * 递归调用应该使用句柄来操作，可以去掉重复这种失误
	 */ 
	// /** 计算物品实际价格 */
	// float CalculateItemEffectivePrice(const UShopItemWidget* ItemWidget);
	//
	// /** 递归计算子节点价值 */
	// float CalculateSubTreeValue(const ITreeNodeInterface* NodeInterface);

	// TODO:还欠缺购买商品自动合成库存已有物品逻辑
	// 我觉得自动合成的逻辑跟这个递归计算子节点价值逻辑差不多，换成remove就ok了
	// 发生购买事件的时候CurrentSelectedItem可以置为 nullptr
	/** 按钮点击事件处理 */
	UFUNCTION()
	void OnBuyButtonClicked();
	
	// DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle& /*ItemHandle*/);
	// TODO:或许需要添加一个FInventoryItemHandle传值，传的时候随便传拉，反正都要关闭按钮和数字置0
	// 禁用按钮，将金额置为0
	void SetButtonNoEnabledAndPriceTextZero(const FInventoryItemHandle& ItemHandle = FInventoryItemHandle::InvalidHandle());

};
