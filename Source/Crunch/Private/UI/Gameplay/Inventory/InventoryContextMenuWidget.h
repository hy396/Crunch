// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InventoryContextMenuWidget.generated.h"

/**
 * 上下文菜单 Widget，用于在物品格子上右键点击时显示“使用”和“出售”按钮
 */
UCLASS()
class UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 获取“使用”按钮的点击事件引用，外部绑定时使用 */
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const;
	/** 获取“出售”按钮的点击事件引用，外部绑定时使用 */
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const;
private:
	/** 绑定到 UMG 编辑器中名为 UseButton 的按钮，用于“使用”物品 */
	UPROPERTY(meta = (BindWidget))
	UButton* UseButton;

	/** 绑定到 UMG 编辑器中名为 SellButton 的按钮，用于“出售”物品 */
	UPROPERTY(meta = (BindWidget))
	UButton* SellButton;
};
