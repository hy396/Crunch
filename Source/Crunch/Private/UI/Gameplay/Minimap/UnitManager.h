// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MinimapWidget.h"
#include "GAS/Core/TGameplayTags.h"
#include "UnitManager.generated.h"

class AActor;
class UTexture2D;
class UMinimapManager;
class AStormCore;
class ACPlayerController;

// 单位信息结构
USTRUCT(BlueprintType)
struct CRUNCH_API FUnitInfo
{
	GENERATED_BODY()
	
	TWeakObjectPtr<AActor> Unit;
	FGameplayTag UnitTypeTag;

	// 添加默认构造函数
	FUnitInfo() : Unit(nullptr) {}

	FUnitInfo(AActor* InUnit, const FGameplayTag& InUnitTypeTag)
		: Unit(InUnit), UnitTypeTag(InUnitTypeTag)
	{
	}
};

/**
 * 单位管理器
 * 负责跟踪游戏中的所有单位，并通知小地图系统更新
 */
UCLASS()
class CRUNCH_API UUnitManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 初始化
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 注册单位
	void RegisterUnit(AActor* Unit);

	// 注销单位
	void UnregisterUnit(AActor* Unit);

	// 更新单位状态
	void UpdateUnitStatus(AActor* Unit);

private:
	// 获取本地玩家控制器
	ACPlayerController* GetLocalPlayerController() const;

	// 所有注册的单位
	UPROPERTY()
	TArray<FUnitInfo> RegisteredUnits;
};