// 幻雨喜欢小猫咪


#include "FrontendUI/FrontendFunctionLibrary.h"

#include "FrontendSettings/FrontendDeveloperSettings.h"


TSoftClassPtr<UWidget_ActivatableBase> UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(
	FGameplayTag InWidgetTag)
{
	// 获取前端UI开发设置实例
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	checkf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag), TEXT("在这标签%s下找不到对应的小部件"), *InWidgetTag.ToString());

	return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FGameplayTag InImageTag)
{
	// 获取前端UI开发设置实例
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	checkf(FrontendDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("根据标签%s找不到图像"), *InImageTag.ToString());
	return FrontendDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}

// ================= Audio Functions =================

TSoftObjectPtr<USoundMix> UFrontendFunctionLibrary::GetMasterSoundMix()
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetMutableDefault<UFrontendDeveloperSettings>();
	checkf(FrontendDeveloperSettings->MasterBusMix, TEXT("MasterBusMix未配置"));
	return FrontendDeveloperSettings->MasterBusMix;
}

TSoftObjectPtr<USoundClass> UFrontendFunctionLibrary::GetSoundClassByTag(FGameplayTag InVolumeTag)
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetMutableDefault<UFrontendDeveloperSettings>();

	checkf(FrontendDeveloperSettings->VolumeBusMap.Contains(InVolumeTag), TEXT("根据标签%s找不到音量设置"), *InVolumeTag.ToString());
	return FrontendDeveloperSettings->VolumeBusMap.FindRef(InVolumeTag);
}