// 幻雨喜欢小猫咪


#include "ShopWidget.h"

#include "ItemTreeWidget.h"
#include "Framework/CAssetManager.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置可聚焦
	SetIsFocusable(true);
	// 加载物品
	LoadShopItems();

	// 绑定列表项生成事件
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetGenerated);

	// 获取玩家的库存组件并存储起来
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	}

	// TODO:25/08/03 添加的购买事件，尚未完全
	// 绑定按钮点击事件
	if (BuyButton)
	{
		BuyButton->OnClicked.AddDynamic(this, &UShopWidget::OnBuyButtonClicked);
		// 开局就禁用按钮
		BuyButton->SetIsEnabled(false);
	}
}

void UShopWidget::LoadShopItems()
{
	// 调用资产管理器的异步加载方法
	// 加载完成后触发 ShopItemLoadFinished 回调
	UCAssetManager::Get().LoadShopItems(
		FStreamableDelegate::CreateUObject(this, &UShopWidget::ShopItemLoadFinished)
		);
}

void UShopWidget::ShopItemLoadFinished()
{
	// 获取所有已加载的商店物品
	TArray<const UPDA_ShopItem*> ShopItems;
	if (UCAssetManager::Get().GetLoadedShopItems(ShopItems))
	{
		// 添加商店物品
		for (const UPDA_ShopItem* ShopItem : ShopItems)
		{
			ShopItemList->AddItem(const_cast<UPDA_ShopItem*>(ShopItem));
		}
	}
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	// 转换为商店物品控件
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ItemWidget)
	{
		// TODO:右键购买的操作将在推出购买按钮的时候取消掉
		// 绑定购买事件到库存系统
		if (OwnerInventoryComponent)
		{
			ItemWidget->OnItemPurchaseIssued.AddUObject(
				OwnerInventoryComponent,
				&UInventoryComponent::TryPurchase);
		}
		
		// 绑定选择事件(鼠标左键)到合成树显示
		ItemWidget->OnShopItemClicked.AddUObject(
			this,
			&UShopWidget::ShowItemCombination);
		
		// 绑定选择事件(鼠标左键)计算商品价格并显示商品价格
		ItemWidget->OnShopItemClicked.AddUObject(
			this,
			&UShopWidget::SelectedShopItem);
		
		// 添加到物品映射表
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);;
	}
}

void UShopWidget::ShowItemCombination(const UShopItemWidget* ItemWidget)
{
	if (CombinationTree)
	{
		// 绘制合成树,以传入物品为Root
		CombinationTree->DrawFromNode(ItemWidget);
	}
}

void UShopWidget::SelectedShopItem(const UShopItemWidget* ItemWidget)
{
	// TODO:再此添加选择的物品，并修改显示的价格
	// if (ItemWidget != nullptr)
	CurrentSelectedItem = ItemWidget;
	float Price = CalculateItemEffectivePrice(ItemWidget);
	PriceDisplayText->SetText(FText::AsNumber(Price));
	// TODO:感觉可以添加一个条件，获取玩家的金额，如果金额足够那么就可以购买，否则不能购买
	// 金额足够允许点击按钮，金额不够禁用按钮
	BuyButton->SetIsEnabled(Price <= OwnerInventoryComponent->GetGold());
}
float UShopWidget::CalculateItemEffectivePrice(const UShopItemWidget* ItemWidget)
{
	if (!ItemWidget) return 0.f;

	float Gold = ItemWidget->GetShopItem()->GetPrice();

	Gold -= CalculateSubTreeValue(ItemWidget);

	return Gold;
}

float UShopWidget::CalculateSubTreeValue(const ITreeNodeInterface* NodeInterface)
{
	if (!NodeInterface) return 0.f;
	if (!OwnerInventoryComponent) return 0.f;
	// 获取下游节点
	TArray<const ITreeNodeInterface*> NextTreeNode = NodeInterface->GetOutputs();
	if (NextTreeNode.Num() == 0)
	{
		return 0.f;
	}
	float Value = 0.f;
	for (const ITreeNodeInterface* NextNode : NextTreeNode)
	{
		if (const UPDA_ShopItem* ShopItem = Cast<const UPDA_ShopItem>(NextNode->GetItemObject()))
		{
			// 寻找背包中是否有该物品,若有则扣除该价格
			if (OwnerInventoryComponent->TryGetItemForShopItem(ShopItem))
			{
				Value += ShopItem->GetPrice();
				continue;
			}
			// 若是没找到，则递归下去找背吧中的物品
			CalculateSubTreeValue(NextNode);
		}
	}
	return Value;
}

void UShopWidget::OnBuyButtonClicked()
{
	if (!CurrentSelectedItem || !CurrentSelectedItem->GetShopItem()) return;
	// 实现购买逻辑
	// 把CurrentSelectedItem商品传给购买逻辑去
	// TODO:因此需要修改TryPurchase的逻辑，目前是25/08/03，待修改
	OwnerInventoryComponent->TryPurchase(CurrentSelectedItem->GetShopItem());
	// 购买后禁用按钮
	BuyButton->SetIsEnabled(false);
	// 金币显示归0
	PriceDisplayText->SetText(FText::AsNumber(0));
}
