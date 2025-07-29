// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Inventory/PDA_ShopItem.h"
#include "ItemToolTip.generated.h"

/**
 * 物品提示控件
 * 功能：显示物品的详细信息提示
 * 包含：
 *   - 物品图标
 *   - 物品名称
 *   - 物品描述
 *   - 物品价格
 * 使用场景：鼠标悬停在物品上时显示
 */
UCLASS()
class CRUNCH_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()
public:
	// 设置要显示提示的物品
	UFUNCTION(BlueprintCallable, Category = "ToolTip")
	void SetItem(const UPDA_ShopItem* Item);
	
	// 设置物品价格（可覆盖默认价格）
	UFUNCTION(BlueprintCallable, Category = "ToolTip")
	void SetPrice(float newPrice);
	
private:
	// 物品图标显示控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> IconImage;

	// 物品名称文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemTitleText;

	// 物品描述文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemDescriptionText;
	
	// RichTextStyleRow 多格式文本块，物品描述文本控件
	// UPROPERTY(meta=(BindWidget))
	// TObjectPtr<URichTextBlock> ItemDescriptionText;
	
	// 物品价格文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemPriceText;
};
