// 幻雨喜欢小猫咪


#include "SplineWidget.h"

void USplineWidget::SetupSpline(const UUserWidget* InStartWidget, const UUserWidget* InEndWidget,
	const FVector2D& InStartPortLocalCoord, const FVector2D& InEndPortLocalCoord, const FVector2D& InStartPortDirection,
	const FVector2D& InEndPortDirection)
{
	// 设置连接的起始控件和目标控件
	StartWidget = InStartWidget;
	EndWidget = InEndWidget;
    
	// 设置连接点在控件内的局部坐标（相对于控件左上角）
	StartPortLocalCoord = InStartPortLocalCoord;
	EndPortLocalCoord = InEndPortLocalCoord;
    
	// 设置样条线在起点和终点的切线方向（控制曲线形状）
	StartPortDirection = InStartPortDirection;
	EndPortDirection = InEndPortDirection;
}

void USplineWidget::SetSplineStyle(const FLinearColor& InColor, float InThickness)
{
	// 设置样条线的视觉样式
	Color = InColor;        // 线条颜色
	Thickness = InThickness; // 线条粗细（像素）
}

int32 USplineWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// 调用父类的绘制方法（确保基础UI元素被正确绘制）
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, 
		OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	
	// 初始化起点和终点位置（默认使用编辑器测试位置）
	FVector2D StartPos = TestStartPos;
	FVector2D EndPos = TestEndPos;
	
	// 如果已设置起始控件和目标控件，则计算实际位置
	if (StartWidget && EndWidget)
	{
		// 将起始点局部坐标转换为当前几何空间的绝对位置
		StartPos = StartWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(StartPortLocalCoord);
        
		// 将终点局部坐标转换为当前几何空间的绝对位置
		EndPos = EndWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(EndPortLocalCoord);
	}
	
	// 使用Slate绘制API创建贝塞尔曲线样条线
	FSlateDrawElement::MakeSpline(
		OutDrawElements,					// 绘制元素列表
		++LayerId,							// 递增图层ID确保在顶层绘制
		AllottedGeometry.ToPaintGeometry(), // 当前控件的几何信息
		StartPos,						// 曲线起点位置
		StartPortDirection,				// 起点切线方向（控制曲线起始斜率）
		EndPos,							// 曲线终点位置
		EndPortDirection,				// 终点切线方向（控制曲线结束斜率）
		Thickness,							// 线条粗细
		ESlateDrawEffect::None,				// 无特殊绘制效果
		Color								// 线条颜色
	);
    
	// 返回最终使用的图层ID
	return LayerId;
}
