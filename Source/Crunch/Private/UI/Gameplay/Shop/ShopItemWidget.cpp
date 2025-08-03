// 幻雨喜欢小猫咪


#include "ShopItemWidget.h"

#include "Components/ListView.h"
#include "Framework/CAssetManager.h"

UUserWidget* UShopItemWidget::GetWidget() const
{
	// 创建一样的商店物品UI
	UShopItemWidget* Copy = CreateWidget<UShopItemWidget>(GetOwningPlayer(), GetClass());

	// 复制商店物品UI数据
	Copy->CopyFromOther(this);
	return Copy;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
	// 获取可合成该物品的材料列表
	const FItemCollection* Collection = UCAssetManager::Get().GetCombinationForItem(GetShopItem());

	// 如果找到了材料，就把它们转换为树节点返回
	if (Collection)
	{
		return ItemsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>{};
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOutputs() const
{
	// 获取合成该物品所需的材料列表
	const FItemCollection* Collection = UCAssetManager::Get().GetIngredientForItem(GetShopItem());

	// 转换为树节点（例如合成树中的“箭头”方向）
	if (Collection)
	{
		return ItemsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>{};
}

const UObject* UShopItemWidget::GetItemObject() const
{
	return ShopItem;
}

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// ShopItem = Cast<UPDA_ShopItem>(ListItemObject);
	// if (!ShopItem) return;
	//
	// SetIcon(ShopItem->GetIcon());
	//
	// SetToolTipWidget(ShopItem);
	// 使用新数据初始化控件
	InitWithShopItem(Cast<UPDA_ShopItem>(ListItemObject));
	
	// 获取所属的ListView控件
	ParentListView = Cast<UListView>(IUserListEntry::GetOwningListView());
}

void UShopItemWidget::CopyFromOther(const UShopItemWidget* OtherWidget)
{
	// 拷贝事件绑定（点击购买、点击选中）
	OnItemPurchaseIssued = OtherWidget->OnItemPurchaseIssued;
	OnShopItemClicked = OtherWidget->OnShopItemClicked;

	// 复制父列表
	ParentListView = OtherWidget->ParentListView;

	// 使用商店物品数据初始化界面
	InitWithShopItem(OtherWidget->GetShopItem());
}

void UShopItemWidget::InitWithShopItem(const UPDA_ShopItem* NewShopItem)
{
	// 设置商店物品数据
	ShopItem = NewShopItem;
	if (!ShopItem) return;
	// 设置图标
	SetIcon(ShopItem->GetIcon());
	// 创建并设置提示信息
	SetToolTipWidget(ShopItem);
}

TArray<const ITreeNodeInterface*> UShopItemWidget::ItemsToInterfaces(const TArray<const UPDA_ShopItem*>& Items) const
{
	// 创建用来存储的树节点的数组
	TArray<const ITreeNodeInterface*> RetInterfaces;

	if (!ParentListView) return RetInterfaces;

	// 遍历物品数组，把它们从 ListView 中转换为 Widget（然后作为树节点返回）
	for (const UPDA_ShopItem* Item : Items)
	{
		// 从ParentListView列表视图中获取与Item数据项关联的UShopItemWidget控件
		// ParentListView存储的就是商店的那个框框
		const UShopItemWidget* ItemWidget = ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(Item);
		if (ItemWidget)
		{
			// 将对应的树节点添加到数组中
			RetInterfaces.Add(ItemWidget);
		}
	}
	return RetInterfaces;
}

void UShopItemWidget::RightButtonClicked()
{
	// 购买
	OnItemPurchaseIssued.Broadcast(GetShopItem());
}

void UShopItemWidget::LeftButtonClicked()
{
	// 选中
	OnShopItemClicked.Broadcast(this);
}
