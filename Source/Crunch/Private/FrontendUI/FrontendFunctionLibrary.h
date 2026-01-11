// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
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


	// ================= Audio Functions =================

	/**
	 * 获取主音频混音器（MasterBusMix）
	 * 从开发者设置中获取并加载 SoundMix（如果已缓存则直接返回）
	 * @return 加载后的 SoundMix，如果加载失败返回 nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library|Audio")
	static TSoftObjectPtr<USoundMix> GetMasterSoundMix();

	/**
	 * 通过GameplayTag获取音频类别（SoundClass）
	 * 从开发者设置中的 VolumeBusMap 查找对应的 SoundClass（如果已缓存则直接返回）
	 * @param InVolumeTag - 音量类型的GameplayTag（如 Frontend.Audio.OverallVolume）
	 * @return 对应的 SoundClass，如果找不到返回 nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library|Audio")
	static TSoftObjectPtr<USoundClass> GetSoundClassByTag(UPARAM(meta=(Categories ="Frontend.Audio")) FGameplayTag InVolumeTag);
};
