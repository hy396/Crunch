// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderActorWidget.generated.h"

class ARenderActor;
class USizeBox;
class UImage;

/**
 * 渲染Actor到纹理的UI控件基类（抽象类）
 * 
 * 功能：
 *  1. 在UI中显示动态渲染的3D内容
 *  2. 自动创建和管理渲染Actor
 *  3. 按固定帧率更新渲染
 * 
 * 使用场景：
 *  - 角色展示界面
 *  - 3D道具预览
 *  - 实时小地图
 */
UCLASS()
class CRUNCH_API URenderActorWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 构建前回调（可用于初始化参数）
	virtual void NativePreConstruct() override;

	// 构建时回调（用于初始化控件、渲染等）
	virtual void NativeConstruct() override;

	// 销毁时回调（用于清理资源）
	virtual void BeginDestroy() override;
private:
	// 配置渲染Actor（设置渲染目标等）
	void ConfigureRenderActor();

	// 派生类实现：生成渲染Actor
	virtual void SpawnRenderActor() PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor, ); 

	// 派生类实现：获取渲染Actor实例
	virtual ARenderActor* GetRenderActor() const PURE_VIRTUAL(URenderActorWidget::GetRenderActor, return nullptr; );

	// 开始渲染捕获（定时刷新渲染）
	void BeginRenderCapture();

	// 更新渲染内容
	void UpdateRender();

	// 停止渲染捕获
	void StopRenderCapture();
	
	// 显示渲染结果的图片控件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DisplayImage;
	
	// 控制渲染区域大小的控件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> RenderSizeBox;

	// 材质参数名，用于绑定渲染目标纹理
	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FName DisplayImageRenderTargetParamName = "RenderTarget";

	// 渲染区域尺寸
	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FVector2D RenderSize;

	// 渲染帧率
	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	int32 FrameRate = 24;

	// 渲染定时器间隔
	float RenderTickInterval;

	// 渲染定时器句柄
	FTimerHandle RenderTimerHandle;

	// 渲染目标纹理
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;
};
