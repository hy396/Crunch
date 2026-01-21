// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
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

	// ================= Input Functions =================

	/**
	 * 根据InputAction获取当前绑定的按键图标
	 * @param MappingName - 输入动作名称
	 * @param InLocalPlayer - 本地玩家，用于获取输入设置
	 * @param DesiredInputType - 期望的输入设备类型（键盘/鼠标 或 手柄）
	 * @return 对应的按键图标画刷，如果找不到则返回空画刷
	 */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library|Input")
	static FSlateBrush GetInputActionIcon(
	UPARAM(Meta=(GetOptions="EnhancedInput.PlayerMappableKeySettings.GetKnownMappingNames")) const FName MappingName,
		const ULocalPlayer* InLocalPlayer,
		ECommonInputType DesiredInputType);
	// virtual const TSet<FPlayerKeyMapping>& FindMappingsInRow( const;

};
