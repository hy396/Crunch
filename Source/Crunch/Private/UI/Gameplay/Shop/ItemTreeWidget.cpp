// 幻雨喜欢小猫咪


#include "ItemTreeWidget.h"

#include "SplineWidget.h"
#include "TreeNodeInterface.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UItemTreeWidget::DrawFromNode(const ITreeNodeInterface* NodeInterface)
{
	if (!NodeInterface) return;

	// 避免重复绘制：检查是否与当前中心节点相同
	if (CurrentCenterItem == NodeInterface->GetItemObject()) return;

	// 清空画布
	ClearTree();
	// 记录中心物品
	CurrentCenterItem = NodeInterface->GetItemObject();

	// 用于计算叶子节点的水平位置
	float NextLeafXPos = 0.0f;
	UCanvasPanelSlot* CenterWidgetPanelSlot = nullptr;
	// 创建中心节点的UI
	UUserWidget* CenterWidget = CreateWidgetForNode(NodeInterface, CenterWidgetPanelSlot);

	// 分别绘制下游节点和上游节点
	TArray<UCanvasPanelSlot*> LowerStreamSlots, UpperStreamSlots;

	// 绘制下游
	DrawStream(
		false,					// bUpperStream = false (下游)
		NodeInterface,          // 起始节点接口
		CenterWidget,           // 中心节点控件
		CenterWidgetPanelSlot,  // 中心节点位置槽
		0,                      // 起始深度
		NextLeafXPos,       // 叶节点位置计数器
		LowerStreamSlots    // 输出：下游节点槽集合
		);
	// 计算总宽度,然后将其居中
	float LowerStreamXMax = NextLeafXPos - NodeSize.X - NodeGap.X;
	// 居中偏移量
	float LowerMoveAmt = 0.f - LowerStreamXMax / 2.0f;
	// 应用下游分支偏移（水平居中）
	for (UCanvasPanelSlot* StreamSlot : LowerStreamSlots)
	{
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D(LowerMoveAmt, 0.f));
	}

	// 刷新水平位置
	NextLeafXPos = 0.f;
	// 向上绘制
	DrawStream(
		true,					// bUpperStream = true (上游)
		NodeInterface,          // 起始节点接口
		CenterWidget,           // 中心节点控件
		CenterWidgetPanelSlot,  // 中心节点位置槽
		0,                      // 起始深度
		NextLeafXPos,       // 叶节点位置计数器
		UpperStreamSlots    // 输出：上游节点槽集合
		);
	// 计算上游分支最大X位置
	float UpperStreamXMax = NextLeafXPos - NodeSize.X - NodeGap.X;
	// 计算上游分支居中偏移量
	float UpperMoveAmt = 0.f - UpperStreamXMax / 2.0f;
	// 应用上游分支偏移（水平居中）
	for (UCanvasPanelSlot* StreamSlot : UpperStreamSlots)
	{
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{UpperMoveAmt, 0.f});
	}

	// 将中心节点置于坐标系原点 (0,0)
	CenterWidgetPanelSlot->SetPosition(FVector2D::Zero());
}

void UItemTreeWidget::DrawStream(bool bUpperStream, const ITreeNodeInterface* StartingNodeInterface,
	UUserWidget* StartingNodeWidget, UCanvasPanelSlot* StartingNodeSlot, int32 StartingNodeDepth,
	float& NextLeafXPosition, TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
	// 节点的生成方式类似于多叉树遍历的前序遍历：采用深度优先策略来遍历，一路走到黑
	
	// 如果bUpperStream为True 获取合成的目标，将向上递归（生成树往上画），否则获取合成的材料物品，向下进行递归（生成树往下画）
	TArray<const ITreeNodeInterface*> NextTreeNode = bUpperStream ? StartingNodeInterface->GetInputs() : StartingNodeInterface->GetOutputs();
	
	// 计算当前节点的垂直位置 = (节点的高度 + 节点之间的垂直间隔) * 当前的深度 * (向上连接的话乘负数，向下连接乘正数)
	float StartingNodeYPos = (NodeSize.Y + NodeGap.Y) * StartingNodeDepth * (bUpperStream ? -1 : 1);

	// 递归的退出条件(遇到叶子节点)
	if (NextTreeNode.Num() == 0)
	{
		// 设置节点的位置
		StartingNodeSlot->SetPosition(FVector2D{NextLeafXPosition, StartingNodeYPos});

		// 更新下一个叶节点的位置:向右移动(节点的宽度 + 水平间距)(用于将父类节点或者是将后面遍历到的兄弟节点往右边移动)
		NextLeafXPosition += NodeSize.X + NodeGap.X;
		return;
	}
	// 累加子节点水平位置
	float NextNodeXPosSum = 0;
	// 遍历所有子节点
	for (const ITreeNodeInterface* NextTreeNodeInterface : NextTreeNode)
	{
		// 子节点的画布插槽
		UCanvasPanelSlot* NextWidgetSlot;
		// 创建子节点UI
		UUserWidget* NextWidget = CreateWidgetForNode(NextTreeNodeInterface, NextWidgetSlot);
		OutStreamSlots.Add(NextWidgetSlot); // 添加节点槽

		// 创建线条连接两个UI
		if (bUpperStream)
		{
			// 如果向上连接的话，由下一个节点(NextWidget)连接到当前节点(StartingNodeWidget)
			CreateConnection(NextWidget, StartingNodeWidget);
		}else
		{
			// 向下连接，当前节点指向下一个节点
			CreateConnection(StartingNodeWidget, NextWidget);
		}

		// 深度优先遍历: 处理子节点的子树
		DrawStream(
			bUpperStream,           // 保持分支方向
			NextTreeNodeInterface,  // 子节点接口
			NextWidget,             // 子节点控件
			NextWidgetSlot,         // 子节点位置槽
			StartingNodeDepth + 1,  // 深度 + 1
			NextLeafXPosition,   // 传递叶节点位置(引用修改)
			OutStreamSlots       // 继续收集节点槽
		);

		// 累加子节点水平位置(累加的时候，该节点已经经过了循环外面的位置计算了，所以这里的值是有的)
		NextNodeXPosSum += NextWidgetSlot->GetPosition().X;
	}
	
	// ===== 位置计算 =====
	// 计算当前节点的水平位置 = 所有子节点位置的平均值
	float AvgXNodePos = NextNodeXPosSum / NextTreeNode.Num();
    
	// 设置当前节点位置(水平居中于子节点, 垂直按深度排列)
	StartingNodeSlot->SetPosition(FVector2D{AvgXNodePos, StartingNodeYPos});
}

void UItemTreeWidget::ClearTree()
{
	// 清空画布
	RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasSlot)
{
	if (!Node) return nullptr;

	// 创建节点的Widget
	UUserWidget* NodeWidget = Node->GetWidget();
	// 将生成的控件添加到Canvas根面板中
	// AddChildToCanvas返回值为画布插槽指针，用于调整布局参数
	OutCanvasSlot = RootPanel->AddChildToCanvas(NodeWidget);
	if (OutCanvasSlot)
	{
		// 指定它的大小、锚点在中心、对齐方式也居中、层级为 1
		OutCanvasSlot->SetSize(NodeSize);
		OutCanvasSlot->SetAnchors(FAnchors(0.5f));
		OutCanvasSlot->SetAlignment(FVector2D(0.5f));
		OutCanvasSlot->SetZOrder(1);
	}
	
	return NodeWidget;
}

void UItemTreeWidget::CreateConnection(const UUserWidget* From, UUserWidget* To)
{
	if (!From || !To) return;

	// 创建线条UI
	USplineWidget* Connection = CreateWidget<USplineWidget>(GetOwningPlayer());
	// 创建画布插槽，并将线条UI添加到画布插槽中，再添加到画布中
	UCanvasPanelSlot* ConnectionSlot = RootPanel->AddChildToCanvas(Connection);
	if (ConnectionSlot)
	{
		// 设置连线底层，确保在节点下面
		ConnectionSlot->SetAnchors(FAnchors{0.f});
		ConnectionSlot->SetAlignment(FVector2D{0.f});
		ConnectionSlot->SetPosition(FVector2D::Zero());
		ConnectionSlot->SetZOrder(0);
	}
	
	// 设置线条的起点、终点、本地偏移和方向
	Connection->SetupSpline(
		From, To,
		SourcePortLocalPos, DestinationPortLocalPos,
		SourcePortDirection, DestinationPortDirection
	);
	// 设置颜色和粗细
	Connection->SetSplineStyle(ConnectionColor, ConnectionThickness);
}
