// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AttributeWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "AttributePanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UAttributePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 构造函数
	virtual void NativeConstruct() override;

	// 获取关闭面板按钮的点击事件委托
	FOnButtonClickedEvent& GetCloseButtonClickedEventDelegate() const;
private:
	// 关闭按钮
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	// 属性面板
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> AttributePanel;

	// 属性类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeWidget> AttributeWidgetClass;

	// 属性数据表
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> AttributeDataTable;

	// // 属性面板弹出动画
	// UPROPERTY(Transient, meta=(BindWidgetAnim))
	// TObjectPtr<UWidgetAnimation> PanelPopupAnimation;
};
