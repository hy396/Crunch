// 幻雨喜欢小猫咪


#include "InventoryItemDragDropOp.h"

#include "InventoryItemWidget.h"
#include "UI/Common/ItemWidget.h"

void UInventoryItemDragDropOp::SetDraggedItem(UInventoryItemWidget* DraggedItem)
{
	// 将被拖拽物件设置为 Payload，便于后续拖拽操作中引用
	Payload = DraggedItem;

	if (DragVisualClass)
	{
		// 创建拖拽视觉控件实例
		UItemWidget* DragItemWidget = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass);
		if (DragItemWidget)
		{
			// 设置拖拽视觉控件的图标为被拖拽物品的图标
			DragItemWidget->SetIcon(DraggedItem->GetIconTexture());
			// 设置默认拖拽视觉控件
			DefaultDragVisual = DragItemWidget;
		}
	}
}
