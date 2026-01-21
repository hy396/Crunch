// 幻雨喜欢小猫咪


#include "FrontendUI/FrontendFunctionLibrary.h"

#include "CommonInputSubsystem.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "EnhancedInputSubsystems.h"
#include "CommonInputSubsystem.h"
#include "Widgets/Options/DataObjects/ListDataObject_KeyRemap.h"
#include "HuanYuDebugHelper.h"

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

// ================= Input Functions =================

FSlateBrush UFrontendFunctionLibrary::GetInputActionIcon(const FName MappingName, const ULocalPlayer* InLocalPlayer,
	const ECommonInputType DesiredInputType)
{
	// 创建一个Slate画刷对象
	FSlateBrush FoundBrush;
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!EISubsystem) return FSlateBrush();
	UEnhancedInputUserSettings* EIUserSettings = EISubsystem->GetUserSettings();
	if (!EIUserSettings) return FSlateBrush();
	FKey My_Key = FKey();
	// 键盘
	FPlayerMappableKeyQueryOptions KeyboardMouseOnly;
	KeyboardMouseOnly.KeyToMatch = EKeys::S;
	KeyboardMouseOnly.bMatchBasicKeyTypes = true;
			
	for (const TPair<FGameplayTag, UEnhancedPlayerMappableKeyProfile*> ProfilePair : EIUserSettings->GetAllSavedKeyProfiles())
	{
		if (My_Key.IsValid()) break;
		UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;
		if (!MappableKeyProfile) return FSlateBrush();
		for (const TPair<FName, FKeyMappingRow>& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
		{
			if (My_Key.IsValid()) break;
			for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
			{	
				if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, KeyboardMouseOnly))
				{
					if (MappingName == KeyMapping.GetMappingName())
					{
						My_Key = KeyMapping.GetCurrentKey();
						break;
					}
				}		
			}
		}
	}
	
	// 获取通用输入子系统
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(InLocalPlayer);
	check(CommonInputSubsystem);

	// 尝试获取输入画刷
	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		FoundBrush, My_Key,
		DesiredInputType, CommonInputSubsystem->GetCurrentGamepadName());

	// 如果没有找到对应画刷，则输出调试信息
	if (!bHasFoundBrush)
	{
		Debug::Print(TEXT("根据这个key：") + My_Key.GetDisplayName().ToString()+
			TEXT("找不到对应的图标，该图标没有设置为空"));
	}
	
	// 返回找到的画刷
	return FoundBrush;
}

