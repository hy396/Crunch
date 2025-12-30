// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_PrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;
/**
 * Abstract 禁止直接实例化，需通过子类继承使用。   DisableNaiveTick 禁用默认的每帧更新机制，需手动管理更新逻辑以优化性能。
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UWidget_PrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InTag) const;

protected:
	UFUNCTION(BlueprintCallable)  // UPARAM(meta=(Categories="Frontend.WidgetStack")  对标签进行过滤，只显示前端的 在引擎中非前端的便签将不再显示  在该类中
	void RegisterWidgetStack(UPARAM(meta=(Categories="Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack);
private:
	// Transient  短暂的  Transient 是优化资源使用和确保数据一致性的重要工具，用于标记"不需要保存的运行时临时数据"，避免不必要的序列化和存储开销。
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> RegisteredWidgetStackMap;
};
