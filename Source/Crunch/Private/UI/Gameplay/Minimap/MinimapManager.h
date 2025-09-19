// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Engine/Texture2D.h"
#include "MinimapManager.generated.h"

class UMinimapWidget;
class AActor;

/**
 * 小地图管理器
 * 负责管理所有小地图相关的逻辑，降低与其他系统的耦合度
 */
UCLASS()
class CRUNCH_API UMinimapManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 注册小地图UI控件
	void RegisterMinimapWidget(UMinimapWidget* Widget);

	// 注销小地图UI控件
	void UnregisterMinimapWidget(UMinimapWidget* Widget);

	// 添加单位到所有小地图
	void AddUnitToMinimaps(AActor* Unit, UTexture2D* Icon, const FLinearColor& Color);

	// 从所有小地图移除单位
	void RemoveUnitFromMinimaps(AActor* Unit);

	// 更新单位在所有小地图上的状态
	void UpdateUnitStatusOnMinimaps(AActor* Unit);

private:
	// 所有注册的小地图控件
	UPROPERTY()
	TArray<TWeakObjectPtr<UMinimapWidget>> RegisteredMinimaps;
};