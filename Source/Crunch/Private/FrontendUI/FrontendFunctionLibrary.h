// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrontendFunctionLibrary.generated.h"
class UWidget_ActivatableBase;

/**
 * 前端功能库
 * 提供前端UI相关的通用函数，包括通过GameplayTag获取Widget类和图像资源的功能
 */
UCLASS()
class UFrontendFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * 通过GameplayTag获取前端Widget软引用类
	 * 从开发者设置中的Widget映射表查找对应的Widget类
	 * @param InWidgetTag - Widget的GameplayTag
	 * @return 对应的Widget软引用类
	 */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftClassPtr<UWidget_ActivatableBase> GetFrontendSoftWidgetClassByTag(UPARAM(meta=(Categories ="Frontend.Widget")) FGameplayTag InWidgetTag);

	/**
	 * 通过GameplayTag获取选项界面软引用图像
	 * 从开发者设置中的图像映射表查找对应的纹理资源
	 * @param InImageTag - 图像的GameplayTag
	 * @return 对应的软引用图像
	 */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftObjectPtr<UTexture2D> GetOptionsSoftImageByTag(UPARAM(meta=(Categories ="Frontend.Image")) FGameplayTag InImageTag);
};
