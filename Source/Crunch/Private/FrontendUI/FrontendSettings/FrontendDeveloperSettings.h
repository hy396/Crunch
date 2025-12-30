// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "FrontendDeveloperSettings.generated.h"

/**
 * 前端UI开发设置类
 * 用于配置和管理前端UI相关的设置，包括Widget映射和选项界面图像资源
 */
class UWidget_ActivatableBase;

UCLASS(Config = Game, defaultconfig, meta=(DisplayName = "Frontend UI Settings"))
class CRUNCH_API UFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/** 前端Widget映射表：通过GameplayTag关联到具体的Widget类 */
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta=(ForceInlineRow, Categories = "Frontend.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> FrontendWidgetMap;

	/** 选项界面软引用图像映射表：通过GameplayTag关联到具体的纹理资源 */
	UPROPERTY(Config, EditAnywhere, Category = "Options Image Reference", meta=(ForceInlineRow, Categories = "Frontend.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;

};
