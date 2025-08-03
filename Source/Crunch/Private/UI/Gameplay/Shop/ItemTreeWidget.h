// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTreeWidget.generated.h"


class UCanvasPanelSlot;
class ITreeNodeInterface;
class UCanvasPanel;

/**
 * @class UItemTreeWidget
 * @brief 合成树的绘制
 */
UCLASS()
class CRUNCH_API UItemTreeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * 从指定的树节点接口开始，绘制整棵树。
	 * @param NodeInterface - 树的“根节点”，实现了 ITreeNodeInterface 的对象（通常代表一个 ShopItem）。
	 */
	void DrawFromNode(const ITreeNodeInterface* NodeInterface);
private:
	/**
	 * 递归绘制一支“上游”或“下游”分支。
	 * @param bUpperStream			- true 表示绘制“上游”（输入源），false 表示“下游”（输出目标）。
	 * @param StartingNodeInterface - 当前分支的起始节点接口。
	 * @param StartingNodeWidget	- 当前节点的UI控件，用于连线起点或终点。
	 * @param StartingNodeSlot		- 当前节点的画布中的插槽。
	 * @param StartingNodeDepth		- 深度（层级），用于垂直位置计算。
	 * @param NextLeafXPosition		- 引用参数，记录绘制下一个叶子节点时应该使用的水平偏移。
	 * @param OutStreamSlots		- 所有节点槽的集合，后续做居中调整。
	 */
	void DrawStream(
		bool bUpperStream,
		const ITreeNodeInterface* StartingNodeInterface,
		UUserWidget* StartingNodeWidget,
		UCanvasPanelSlot* StartingNodeSlot,
		int32 StartingNodeDepth,
		float& NextLeafXPosition,
		TArray<UCanvasPanelSlot*>& OutStreamSlots
	);
	
	/** 清空当前画布上的所有节点和连线 */
	void ClearTree();

	/**
	 * 根据一个节点接口创建对应的 Widget 并添加到 CanvasPanel，
	 * 同时返回该 Widget 在 CanvasPanel 上的 Slot 以便后续定位。
	 * @param Node - 要显示的数据节点接口。
	 * @param OutCanvasSlot - 输出参数，生成的 CanvasPanelSlot。
	 * @return 新创建的 UUserWidget*，或 nullptr。
	 */
	UUserWidget* CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasSlot);

	/**
	 * 创建一条从 "From" Widget 到 "To" Widget 的连线（SplineWidget）。
	 * @param From - 起始节点 Widget。
	 * @param To - 目标节点 Widget。
	 */
	void CreateConnection(const UUserWidget* From, UUserWidget* To);

	// 画布根节点
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootPanel;

	/** 用来记录当前树的中心物品，避免重复绘制同一个树 */
	UPROPERTY()
	TObjectPtr<const UObject> CurrentCenterItem;

	/** 节点尺寸（宽高），可在编辑器中调整 */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D NodeSize = FVector2D{ 60.f };

	/** 节点之间的水平和垂直间隔 */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D NodeGap = FVector2D{ 16.f, 30.f};

	/** 连线颜色 */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FLinearColor ConnectionColor = FLinearColor{0.8f, 0.8f, 0.8f, 1.f};

	/** 连线粗细 */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	float ConnectionThickness = 3.f;

	/** 连线起点相对于节点大小的本地坐标（百分比） */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D SourcePortLocalPos = FVector2D{ 0.5f, 0.9f };

	/** 连线终点相对于节点大小的本地坐标（百分比） */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D DestinationPortLocalPos = FVector2D{ 0.5f, 0.1f };

	/** 起点连线方向（角度） */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D SourcePortDirection = FVector2D{ 0.f, 90.f };

	/** 终点连线方向（角度） */
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D DestinationPortDirection = FVector2D{ 0.f, 90.f };
};
