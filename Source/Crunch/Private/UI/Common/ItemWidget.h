// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 初始化控件
	virtual void NativeConstruct() override;

	// 设置物品图标
	virtual void SetIcon(UTexture2D* IconTexture);

protected:
	// 创建并设置ToolTip控件
	// UItemToolTip* SetToolTipWidget(const UPA_ShopItem* Item);
	
	// 获取图标控件（子类可访问）
	UImage* GetItemIcon() const { return ItemIcon; }

private:
	// 物品图标显示控件（与蓝图中的Image组件绑定）
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;

	// ToolTip控件类（在编辑器中设置默认类型）
	// UPROPERTY(EditDefaultsOnly, Category = "ToolTip")
	// TSubclassOf<UItemToolTip> ItemToolTipClass;

	// 鼠标按下事件处理
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	// 鼠标释放事件处理
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// 右键点击响应（子类可重写实现具体逻辑）
	virtual void RightButtonClicked();
	
	// 左键点击响应（子类可重写实现具体逻辑）
	virtual void LeftButtonClicked();

};
