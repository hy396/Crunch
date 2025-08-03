// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplineWidget.generated.h"

/**
 * 自定义样条线控件，用于在两个UI控件之间绘制贝塞尔曲线连接线
 * 常用于技能树、节点图、流程图等UI连接线绘制
 */
UCLASS()
class CRUNCH_API USplineWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * 初始化样条线连接参数
	 * @param InStartWidget 起始控件
	 * @param InEndWidget 目标控件
	 * @param InStartPortLocalCoord 起始点在起始控件内的局部坐标（相对于控件左上角）
	 * @param InEndPortLocalCoord 目标点在目标控件内的局部坐标
	 * @param InStartPortDirection 起始点切线方向（控制曲线形状）
	 * @param InEndPortDirection 目标点切线方向（控制曲线形状）
	 */
	void SetupSpline(
		const UUserWidget* InStartWidget,
		const UUserWidget* InEndWidget,
		const FVector2D& InStartPortLocalCoord,
		const FVector2D& InEndPortLocalCoord,
		const FVector2D& InStartPortDirection,
		const FVector2D& InEndPortDirection
	);

	/**
	 * 设置样条线显示样式
	 * @param InColor 线条颜色
	 * @param InThickness 线条粗细（像素）
	 */
	void SetSplineStyle(const FLinearColor& InColor, float InThickness);
private:
	// 重写原生绘制函数
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	// 编辑器测试用起点位置（开发调试使用）
	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D TestStartPos;

	// 编辑器测试用终点位置（开发调试使用）
	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D TestEndPos = FVector2D{100.f, 100.f};

	// 样条线颜色
	UPROPERTY(EditAnywhere, Category = "Spline")
	FLinearColor Color = FLinearColor::White;

	// 样条线粗细（像素单位）
	UPROPERTY(EditAnywhere, Category = "Spline")
	float Thickness = 3.f;

	// 起始控件引用
	UPROPERTY()
	const UUserWidget* StartWidget;

	// 目标控件引用
	UPROPERTY()
	const UUserWidget* EndWidget;

	// 起始点在起始控件内的局部坐标
	FVector2D StartPortLocalCoord;

	// 目标点在目标控件内的局部坐标
	FVector2D EndPortLocalCoord;

	// 起始点切线方向（控制曲线起始斜率）
	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D StartPortDirection;

	// 目标点切线方向（控制曲线结束斜率）
	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D EndPortDirection;
};
