// 幻雨喜欢小猫咪


#include "AttributePanelWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/UniformGridSlot.h"

void UAttributePanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 清空属性面板
	AttributePanel->ClearChildren();
	int32 Index = 0;
	APawn* OwnerPlayerPawn = GetOwningPlayerPawn();
	if (!OwnerPlayerPawn) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPlayerPawn);
	if (!OwnerASC) return;
	for (const TPair<FName, uint8*>& DataPair : AttributeDataTable->GetRowMap())
	{
		const FCharacterAttributeData* CachedData = AttributeDataTable->FindRow<FCharacterAttributeData>(DataPair.Key, "");
		if (CachedData)
		{
			UAttributeWidget* NewAttributeWidget = CreateWidget<UAttributeWidget>(this, AttributeWidgetClass);
			// 绑定属性，返回真值则绑定成功
			if (NewAttributeWidget->BindForAscToAttribute(OwnerASC, CachedData->AttributeName, CachedData->AttributeValue))
			{
				// 添加到网格布局
				if (UUniformGridSlot* NewGridSlot = AttributePanel->AddChildToUniformGrid(NewAttributeWidget))
				{
					// 计算行列位置
					int32 Row = Index / 2;
					int32 Column = Index % 2;

					NewGridSlot->SetRow(Row);
					NewGridSlot->SetColumn(Column);
					// 填充
					NewGridSlot->SetHorizontalAlignment(HAlign_Fill);
				}
				++Index;
			}
		}
	}
	
}

FOnButtonClickedEvent& UAttributePanelWidget::GetCloseButtonClickedEventDelegate() const
{
	return CloseButton->OnClicked;
}
