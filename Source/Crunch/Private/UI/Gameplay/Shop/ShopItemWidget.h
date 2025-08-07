// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "TreeNodeInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Inventory/PDA_ShopItem.h"
#include "UI/Common/Items/ItemWidget.h"
#include "ShopItemWidget.generated.h"


class UShopItemWidget;

// 声明委托：当物品购买请求发出时触发（参数：商店物品数据资产）
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssused, const UPDA_ShopItem*);

// 声明委托：当商店物品被选中时触发（参数：商店物品控件实例）
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 * 商店物品UI控件
 * 功能：
 *   - 继承基础物品控件功能
 *   - 实现列表项接口（用于ListView）
 *   - 实现树节点接口（用于合成树展示）
 *   - 处理购买和选择事件
 * 使用场景：商店界面、合成系统界面
 */
UCLASS()
class UShopItemWidget : public UItemWidget, 
						public IUserObjectListEntry,
						public ITreeNodeInterface
{
	GENERATED_BODY()
public:
	// 委托：物品购买请求发出时广播
	FOnItemPurchaseIssused OnItemPurchaseIssued;
	
	// 委托：物品被点击选择时广播
	FOnShopItemSelected OnShopItemClicked;

	//~ Begin ITreeNodeInterface 接口实现
	// 获取当前控件实例（树节点接口要求）
	virtual UUserWidget* GetWidget() const override;
	
	// 获取输入节点（合成树中的材料项）
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	
	// 获取输出节点（合成树中的产出项）
	virtual TArray<const ITreeNodeInterface*> GetOutputs() const override;
	
	// 获取关联的数据对象（商店物品资产）
	virtual const UObject* GetItemObject() const override;
	//~ End ITreeNodeInterface 接口实现
	
	
	//~ Begin IUserObjectListEntry 接口实现
	// 当列表项绑定数据对象时调用（通常为UPA_ShopItem实例）
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//~ End IUserObjectListEntry 接口实现

	// 获取当前绑定的商店物品数据
	FORCEINLINE const UPDA_ShopItem* GetShopItem() const { return ShopItem; }
private:
	// 从另一个商店物品控件复制数据（用于列表项重用）
	void CopyFromOther(const UShopItemWidget* OtherWidget);
	
	// 使用商店物品数据初始化控件
	void InitWithShopItem(const UPDA_ShopItem* NewShopItem);
	
	/**
	 * 将商店物品数组转换为树节点接口数组
	 * @param Items 商店物品数组
	 * @return 对应的树节点接口数组
	 */
	TArray<const ITreeNodeInterface*> ItemsToInterfaces(const TArray<const UPDA_ShopItem*>& Items) const;

	// 父级ListView控件（此物品所属的列表视图）
	UPROPERTY()
	TObjectPtr<const class UListView> ParentListView;
	
	// 当前绑定的商店物品数据资产
	UPROPERTY()
	TObjectPtr<const UPDA_ShopItem> ShopItem;

	//~ Begin UItemWidget 重写
	// 右键点击处理：触发购买委托
	virtual void RightButtonClicked() override;
	
	// 左键点击处理：触发选择委托
	virtual void LeftButtonClicked() override;
	//~ End UItemWidget 重写
};
