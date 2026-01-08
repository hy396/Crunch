// 幻雨喜欢小猫咪


#include "OptionsDataRegistry.h"

#include "OptionsDataInteractionHelper.h"
#include "DataObjects/ListDataObject_Collection.h"
#include "DataObjects/ListDataObject_Scalar.h"
#include "DataObjects/ListDataObject_String.h"
#include "DataObjects/ListDataObject_StringResolution.h"
#include "FrontendUI/FrontendFunctionLibrary.h"
#include "FrontendUI/Core/FrontendGameplayTags.h"
#include "FrontendUI/FrontendSettings/FrontendGameUserSettings.h"
#include "Internationalization/StringTableRegistry.h"


#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, SetterOrGetterFuncName))

#define GET_DESCRIPTION(InKey) LOCTABLE("/Game/Widgets/FrontendUI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", InKey)

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	// 清空可能存在的旧数据（防御性编程）
	RegisteredOptionsTabCollections.Empty();

	// 初始化各个功能模块的选项集合
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InOwningLocalPlayer);

	// 注：此处可扩展，例如从配置文件动态加载自定义 Tab
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(
	const FName& InSelectedTabID) const
{
	// 通过传入的Tab ID查找对应的选项集合
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](UListDataObject_Collection* AvailableTabCollection)->bool
		{
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);
	// 确保找到了对应的Tab集合，否则抛出断言错误
	checkf(FoundTabCollectionPtr, TEXT("在%s的tab下没有找到ID"), *InSelectedTabID.ToString());

	UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;
	// return FoundTabCollection->GetAllChildListData();  这种只能获取最顶部的数据，如果有子类则不能展示

	// 创建数组用于存储所有子列表项（包括递归的子项）
	TArray<UListDataObject_Base*> AllChildListItems;
	// 遍历找到的Tab集合中的所有子数据
	for (UListDataObject_Base* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData) continue;

		// 添加当前子数据到结果数组
		AllChildListItems.Add(ChildListData);

		// 如果当前子数据还有子数据，则递归查找所有子数据
		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData, AllChildListItems);
		}
	}
	return AllChildListItems;
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	// 我暂时不想要这个，我不太需要
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Gameplay")));
	//创建一个共享指针  构建数据交互助手代码
	// TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper =MakeShared<FOptionsDataInteractionHelper>(
	// 	GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, GetCurrentGameDifficulty));
		
	// 设置游戏难度数据
	{
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName("GameDifficulty"));
		GameDifficulty->SetDataDisplayName(FText::FromString(TEXT("Difficulty")));
		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT("您可以调整游戏难度。\n\n<Bold>简单：</>聚焦于游戏故事，最简单的游戏体验。\n\n<Bold>一般：</>提供稍微不同的战斗体验。\n\n<Bold>高难度：</>提供难度比较高的战斗体验。\n\n<Bold>最高难度：</>提供难度非常高的战斗体验，谨慎选择。")));
		GameDifficulty->AddDynamicOption(TEXT("简单"), FText::FromString(TEXT("简单")));
		GameDifficulty->AddDynamicOption(TEXT("一般"), FText::FromString(TEXT("一般")));
		GameDifficulty->AddDynamicOption(TEXT("高难度"), FText::FromString(TEXT("高难度")));
		GameDifficulty->AddDynamicOption(TEXT("最高难度"), FText::FromString(TEXT("最高难度")));
		GameDifficulty->SetDefaultValueFromString(TEXT("一般"));  // 设置默认选项
		// 给UListDataObject_Value中的两个共享指针进行赋值
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplySettingsImmediately(true);
		
		GameplayTabCollection->AddChildListData(GameDifficulty);
		
	}
	// 用来测试 凑条数
	{
		UListDataObject_String* TestItem = NewObject<UListDataObject_String>();
		TestItem->SetDataID(FName("TestItem"));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Image Item")));
		TestItem->SetSoftDescriptionImage(UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FrontendGameplayTags::Image::TestImage));
		TestItem->SetDescriptionRichText(FText::FromString(TEXT("在项目设置中指定要显示的任何图像")));
		GameplayTabCollection->AddChildListData(TestItem);
	}
	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("音频")));
	// Volume Category
	{
		UListDataObject_Collection* VolumeCategoryCollection = NewObject<UListDataObject_Collection>();
		VolumeCategoryCollection->SetDataID(FName("VolumeCategoryCollection"));
		VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("音量"))); // 音量

		AudioTabCollection->AddChildListData(VolumeCategoryCollection);

		// 全局音量
		{
			UListDataObject_Scalar* OverallVolume = NewObject<UListDataObject_Scalar>();
			OverallVolume->SetDataID(FName("OverallVolume"));
			OverallVolume->SetDataDisplayName(FText::FromString(TEXT("全局音量")));
			OverallVolume->SetDescriptionRichText(FText::FromString(TEXT("调整游戏的整体音量大小。\n\n<Bold>背景音乐：</>控制游戏背景音乐、主题曲等音乐元素的音量。\n\n<Bold>音效：</>控制游戏中的各种音效，如脚步声、武器声、环境音效等。\n\n<Bold>对话：</>控制游戏中角色对话的音量。\n\n将滑块向右移动可增加音量，向左移动可降低音量。建议根据您的音频设备和环境噪音水平进行调整，以获得最佳的游戏体验。")));
			OverallVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));  // UI中范围是0到1
			OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));   // 配置文件中是0到2，也就是UI中的0.5对应配置文件中的1
			OverallVolume->SetSliderStepSize(0.01f);
			OverallVolume->SetDefaultValueFromString(LexToString(1.f));  // OverallVolume对象的数据默认值 该方法是父类里面的
			OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			OverallVolume->SetNumberedFormattingOptions(UListDataObject_Scalar::NoDecimal());  // NoDecimal :50%  // One Decimal: 50.f%

			// 给UListDataObject_Value中的两个共享指针进行赋值
			OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
			OverallVolume->SetShouldApplySettingsImmediately(true);
			VolumeCategoryCollection->AddChildListData(OverallVolume);
		}
		// 音乐音量
		{
			UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
			MusicVolume->SetDataID(FName("MusicVolume"));
			MusicVolume->SetDataDisplayName(FText::FromString(TEXT("音乐音量")));
			MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("调整游戏背景音乐的音量大小。\n\n<Bold>背景音乐：</>控制游戏中的背景音乐、主题曲等音乐元素的音量。\n\n<Bold>环境音乐：</>控制游戏中的环境音乐和氛围音乐。\n\n<Bold>主题曲：</>控制游戏主题曲的音量。\n\n将滑块向右移动可增加音乐音量，向左移动可降低音乐音量。建议根据个人喜好和环境情况调整，以获得最佳的音乐体验。")));
			MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));  // UI中范围是0到1
			MusicVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));   // 配置文件中是0到2，也就是UI中的0.5对应配置文件中的1
			MusicVolume->SetSliderStepSize(0.01f);
			MusicVolume->SetDefaultValueFromString(LexToString(1.f));  // MusicVolume对象的数据默认值 该方法是父类里面的
			MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			MusicVolume->SetNumberedFormattingOptions(UListDataObject_Scalar::NoDecimal());  // NoDecimal :50%  // One Decimal: 50.f%

			// 给UListDataObject_Value中的两个共享指针进行赋值
			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
			MusicVolume->SetShouldApplySettingsImmediately(true);
			VolumeCategoryCollection->AddChildListData(MusicVolume);
		}
		// 音效音量
		{
			UListDataObject_Scalar* SoundFXVolume = NewObject<UListDataObject_Scalar>();
			SoundFXVolume->SetDataID(FName("SoundFXVolume"));
			SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("音效音量")));
			SoundFXVolume->SetDescriptionRichText(FText::FromString(TEXT("调整游戏音效的音量大小。\n\n<Bold>脚步声：</>控制角色移动时的脚步声。\n\n<Bold>武器声：</>控制武器射击、挥舞等声音效果。\n\n<Bold>环境音效：</>控制环境中的各种音效，如风声、水声等。\n\n<Bold>交互音效：</>控制与物体交互时的声音效果。\n\n适当调整音效音量可以增强游戏的沉浸感和临场感。将滑块向右移动可增加音效音量，向左移动可降低音效音量。")));
			SoundFXVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));  // UI中范围是0到1
			SoundFXVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));   // 配置文件中是0到2，也就是UI中的0.5对应配置文件中的1
			SoundFXVolume->SetSliderStepSize(0.01f);
			SoundFXVolume->SetDefaultValueFromString(LexToString(1.f));  // SoundFXVolume对象的数据默认值 该方法是父类里面的
			SoundFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			SoundFXVolume->SetNumberedFormattingOptions(UListDataObject_Scalar::NoDecimal());  // NoDecimal :50%  // One Decimal: 50.f%

			// 给UListDataObject_Value中的两个共享指针进行赋值
			SoundFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSoundFXVolume));
			SoundFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSoundFXVolume));
			SoundFXVolume->SetShouldApplySettingsImmediately(true);
			VolumeCategoryCollection->AddChildListData(SoundFXVolume);
		}
	}
	// 音频类别
	{
		UListDataObject_Collection* SoundCategoryCollection = NewObject<UListDataObject_Collection>();
		SoundCategoryCollection->SetDataID(FName("SoundCategoryCollection"));
		SoundCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("声音")));

		AudioTabCollection->AddChildListData(SoundCategoryCollection);
		// 允许后台音频
		{
			UListDataObject_StringBool* AllowBackgroundAudio = NewObject<UListDataObject_StringBool>();
			AllowBackgroundAudio->SetDataID(FName("AllowBackgroundAudio"));
			AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("允许后台音频")));
			AllowBackgroundAudio->SetDescriptionRichText(FText::FromString(TEXT("允许游戏在后台运行时继续播放音频。\n\n<Bold>启用：</>游戏在后台时继续播放音频，适合多任务处理。\n\n<Bold>禁用：</>游戏进入后台时停止播放音频，节省系统资源。\n\n注意：某些系统或设备可能不支持后台音频播放功能。")));
			AllowBackgroundAudio->OverrideTrueDisplayText(FText::FromString(TEXT("已启用")));
			AllowBackgroundAudio->OverrideFalseDisplayText(FText::FromString(TEXT("已禁用")));
			AllowBackgroundAudio->SetFalseAsDefaultValue();
			AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundAudio));
			AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundAudio));
			AllowBackgroundAudio->SetShouldApplySettingsImmediately(true);

			SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
		}
		// 使用HDR音频
		{
			UListDataObject_StringBool* UseHDRAudioMode = NewObject<UListDataObject_StringBool>();
			UseHDRAudioMode->SetDataID(FName("UseHDRAudioMode"));
			UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("使用HDR音频模式")));
			UseHDRAudioMode->SetDescriptionRichText(FText::FromString(TEXT("启用高动态范围(HDR)音频模式以获得更好的音频体验。\n\n<Bold>启用：</>激活HDR音频处理，提供更丰富的音频细节和动态范围。\n\n<Bold>禁用：</>使用标准音频处理模式。\n\nHDR音频需要支持的硬件和软件环境才能正常工作，如果您的设备不支持可能会导致音频问题。")));
			UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("已启用")));
			UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("已禁用")));
			UseHDRAudioMode->SetFalseAsDefaultValue();
			UseHDRAudioMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
			UseHDRAudioMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
			UseHDRAudioMode->SetShouldApplySettingsImmediately(true);

			SoundCategoryCollection->AddChildListData(UseHDRAudioMode);
		}
	}

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("画面")));
	
	UListDataObject_StringEnum* CreatedWindowMode = nullptr;
	// Display Category
	{
		UListDataObject_Collection* DisplayCategoryCollection = NewObject<UListDataObject_Collection>();
		DisplayCategoryCollection->SetDataID(FName("DisplayCategoryCollection"));
		DisplayCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("显示")));

		VideoTabCollection->AddChildListData(DisplayCategoryCollection);

		FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
		PackagedBuildOnlyCondition.SetEditConditionFunc(
			[]()->bool
			{
				const bool bIsInEditor = GIsEditor || GIsPlayInEditorWorld;
				return !bIsInEditor;  // 如果在编辑器中则返回false
			});
#if WITH_EDITOR	
		PackagedBuildOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>该设置只有在打包后的程序中进行设置</>"));
#endif

		// Window mode
		{
			// const FText WindowModeDesc = LOCTABLE("/Game/BP/UI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", "WindowModeDescKey");
			UListDataObject_StringEnum* WindowMode = NewObject<UListDataObject_StringEnum>();
			WindowMode->SetDataID(FName("WindowMode"));
			WindowMode->SetDataDisplayName(FText::FromString(TEXT("窗口模式")));
			// WindowMode->SetDescriptionRichText(FText::FromString(TEXT("这是显示屏框框模式的描述")));
			// WindowMode->SetDescriptionRichText(WindowModeDesc);
			WindowMode->SetDescriptionRichText(GET_DESCRIPTION("WindowModeDescKey"));
			WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("全屏模式")));  // 窗口处于真正的全屏模式
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("无边框窗口"))); // 窗口没有边框，占据了屏幕的整个区域
			WindowMode->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("窗口模式")));  // The window has a border and may not take up the entire screen area
			WindowMode->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
			WindowMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
			WindowMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));
			WindowMode->SetShouldApplySettingsImmediately(true);

			WindowMode->AddEditCondition(PackagedBuildOnlyCondition);
			CreatedWindowMode = WindowMode;
			
			DisplayCategoryCollection->AddChildListData(WindowMode);
		}
		// 屏幕清晰度
		{
			UListDataObject_StringResolution* ScreenResolution = NewObject<UListDataObject_StringResolution>();
			ScreenResolution->SetDataID(FName("ScreenResolution"));
			ScreenResolution->SetDataDisplayName(FText::FromString(TEXT("屏幕分辨率")));
			ScreenResolution->SetDescriptionRichText(GET_DESCRIPTION("ScreenResolutionsDescKey"));
			ScreenResolution->InitResolutionValues();
			ScreenResolution->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
			ScreenResolution->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
			ScreenResolution->SetShouldApplySettingsImmediately(true);
			
			ScreenResolution->AddEditCondition(PackagedBuildOnlyCondition);

			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunc(
				[CreatedWindowMode]()->bool
				{
					const bool bIsBoderlessWindow = CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::WindowedFullscreen;
					return !bIsBoderlessWindow;
				});

			WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>在无边框模式下无法调整屏幕分辨率，分辨率为最大的那一项</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(ScreenResolution->GetMaximumAllowedResolution());
			ScreenResolution->AddEditCondition(WindowModeEditCondition);
			ScreenResolution->AddEditDependencyData(CreatedWindowMode);
			DisplayCategoryCollection->AddChildListData(ScreenResolution);
		}
	}

	// 图形  图样类别
	{
		UListDataObject_Collection* GraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
		GraphicsCategoryCollection->SetDataID(FName("GraphicsCategoryCollection"));
		GraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("图形")));

		VideoTabCollection->AddChildListData(GraphicsCategoryCollection);

		// 显示伽玛 亮度
		{
			UListDataObject_Scalar* DisplayGamma = NewObject<UListDataObject_Scalar>();
			DisplayGamma->SetDataID(FName("DisplayGamma"));
			DisplayGamma->SetDataDisplayName(FText::FromString(TEXT("亮度")));
			DisplayGamma->SetDescriptionRichText(GET_DESCRIPTION("DisplayGammaDescKey"));
			DisplayGamma->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f)); // 默认值为2.2f
			DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f)); // 默认值为2.2f
			DisplayGamma->SetSliderStepSize(0.01f);// 手柄操作时滑块步长
			DisplayGamma->SetDisplayNumericType(ECommonNumericType::Percentage);
			DisplayGamma->SetNumberedFormattingOptions(UListDataObject_Scalar::NoDecimal());
			DisplayGamma->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
			DisplayGamma->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));
			DisplayGamma->SetDefaultValueFromString(LexToString(2.2f));
			
			DisplayGamma->SetShouldApplySettingsImmediately(true);

			GraphicsCategoryCollection->AddChildListData(DisplayGamma);
		}

		UListDataObject_StringInteger* CreatedOverallQuality = nullptr;
		// 全局质量
		{
			UListDataObject_StringInteger* OverallQuality = NewObject<UListDataObject_StringInteger>();
			OverallQuality->SetDataID(FName("OverallQuality"));
			OverallQuality->SetDataDisplayName(FText::FromString(TEXT("全局质量")));
			OverallQuality->SetDescriptionRichText(GET_DESCRIPTION("OverallQualityDescKey"));
			OverallQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			OverallQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			OverallQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			OverallQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			OverallQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			OverallQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
			OverallQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
			OverallQuality->SetShouldApplySettingsImmediately(true);
			
			GraphicsCategoryCollection->AddChildListData(OverallQuality);
			CreatedOverallQuality = OverallQuality;
		}
		// 分辨率缩放比例
		{
			UListDataObject_Scalar* ResolutionScale = NewObject<UListDataObject_Scalar>();
			ResolutionScale->SetDataID(FName("ResolutionScale"));
			ResolutionScale->SetDataDisplayName(FText::FromString(TEXT("3D 屏幕比例")));
			ResolutionScale->SetDescriptionRichText(GET_DESCRIPTION("ResolutionScaleDescKey"));
			ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetSliderStepSize(0.01f);// 手柄操作时滑块步长
			ResolutionScale->SetDisplayNumericType(ECommonNumericType::Percentage);
			ResolutionScale->SetNumberedFormattingOptions(UListDataObject_Scalar::NoDecimal());
			ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
			ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));
			ResolutionScale->SetShouldApplySettingsImmediately(true);

			ResolutionScale->AddEditDependencyData(CreatedOverallQuality);
			GraphicsCategoryCollection->AddChildListData(ResolutionScale);
		}
		// 全局光照质量
		{
			UListDataObject_StringInteger* GlobalIlluminationQuality = NewObject<UListDataObject_StringInteger>();
			GlobalIlluminationQuality->SetDataID(FName("GlobalIlluminationQuality"));
			GlobalIlluminationQuality->SetDataDisplayName(FText::FromString(TEXT("全局光照")));
			GlobalIlluminationQuality->SetDescriptionRichText(GET_DESCRIPTION("GlobalIlluminationQualityDescKey"));
			GlobalIlluminationQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			GlobalIlluminationQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			GlobalIlluminationQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			GlobalIlluminationQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			GlobalIlluminationQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			GlobalIlluminationQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);
			GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
		}
		// 阴影质量
		{
			UListDataObject_StringInteger* ShadowQuality = NewObject<UListDataObject_StringInteger>();
			ShadowQuality->SetDataID(FName("ShadowQuality"));
			ShadowQuality->SetDataDisplayName(FText::FromString(TEXT("阴影质量")));
			ShadowQuality->SetDescriptionRichText(GET_DESCRIPTION("ShadowQualityDescKey"));
			ShadowQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			ShadowQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			ShadowQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			ShadowQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			ShadowQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			ShadowQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
			ShadowQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));
			ShadowQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			ShadowQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ShadowQuality);
			GraphicsCategoryCollection->AddChildListData(ShadowQuality);
		}
		// 抗锯齿质量
		{
			UListDataObject_StringInteger* AntiAliasingQuality = NewObject<UListDataObject_StringInteger>();
			AntiAliasingQuality->SetDataID(FName("AntiAliasingQuality"));
			AntiAliasingQuality->SetDataDisplayName(FText::FromString(TEXT("抗锯齿质量")));
			AntiAliasingQuality->SetDescriptionRichText(GET_DESCRIPTION("AntiAliasingDescKey"));
			AntiAliasingQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			AntiAliasingQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			AntiAliasingQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			AntiAliasingQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			AntiAliasingQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			AntiAliasingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAntiAliasingQuality));
			AntiAliasingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAntiAliasingQuality));
			AntiAliasingQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			AntiAliasingQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(AntiAliasingQuality);
			GraphicsCategoryCollection->AddChildListData(AntiAliasingQuality);
		}
		// 视图距离质量
		{
			UListDataObject_StringInteger* ViewDistanceQuality = NewObject<UListDataObject_StringInteger>();
			ViewDistanceQuality->SetDataID(FName("ViewDistanceQuality"));
			ViewDistanceQuality->SetDataDisplayName(FText::FromString(TEXT("视距质量")));
			ViewDistanceQuality->SetDescriptionRichText(GET_DESCRIPTION("ViewDistanceDescKey"));
			ViewDistanceQuality->AddIntegerOption(0, FText::FromString(TEXT("近")));
			ViewDistanceQuality->AddIntegerOption(1, FText::FromString(TEXT("中")));
			ViewDistanceQuality->AddIntegerOption(2, FText::FromString(TEXT("远")));
			ViewDistanceQuality->AddIntegerOption(3, FText::FromString(TEXT("很远")));
			ViewDistanceQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			ViewDistanceQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
			ViewDistanceQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
			ViewDistanceQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			ViewDistanceQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ViewDistanceQuality);
			GraphicsCategoryCollection->AddChildListData(ViewDistanceQuality);
		}
		// 纹理质量
		{
			UListDataObject_StringInteger* TextureQuality = NewObject<UListDataObject_StringInteger>();
			TextureQuality->SetDataID(FName("TextureQuality"));
			TextureQuality->SetDataDisplayName(FText::FromString(TEXT("纹理质量")));
			TextureQuality->SetDescriptionRichText(GET_DESCRIPTION("TextureQualityDescKey"));
			TextureQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			TextureQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			TextureQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			TextureQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			TextureQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			TextureQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
			TextureQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));
			TextureQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			TextureQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(TextureQuality);
			GraphicsCategoryCollection->AddChildListData(TextureQuality);
		}
		// 视觉效果质量
		{
			UListDataObject_StringInteger* VisualEffectQuality = NewObject<UListDataObject_StringInteger>();
			VisualEffectQuality->SetDataID(FName("VisualEffectQuality"));
			VisualEffectQuality->SetDataDisplayName(FText::FromString(TEXT("视觉效果质量")));
			VisualEffectQuality->SetDescriptionRichText(GET_DESCRIPTION("VisualEffectQualityDescKey"));
			VisualEffectQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			VisualEffectQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			VisualEffectQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			VisualEffectQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			VisualEffectQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			VisualEffectQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
			VisualEffectQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
			VisualEffectQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			VisualEffectQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(VisualEffectQuality);
			GraphicsCategoryCollection->AddChildListData(VisualEffectQuality);
		}
		// 反射质量
		{
			UListDataObject_StringInteger* ReflectionQuality = NewObject<UListDataObject_StringInteger>();
			ReflectionQuality->SetDataID(FName("ReflectionQuality"));
			ReflectionQuality->SetDataDisplayName(FText::FromString(TEXT("反射质量")));
			ReflectionQuality->SetDescriptionRichText(GET_DESCRIPTION("ReflectionQualityDescKey"));
			ReflectionQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			ReflectionQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			ReflectionQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			ReflectionQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			ReflectionQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			ReflectionQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
			ReflectionQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));
			ReflectionQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			ReflectionQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ReflectionQuality);
			GraphicsCategoryCollection->AddChildListData(ReflectionQuality);
		}
		// 后处理质量
		{
			UListDataObject_StringInteger* PostProcessingQuality = NewObject<UListDataObject_StringInteger>();
			PostProcessingQuality->SetDataID(FName("PostProcessingQuality"));
			PostProcessingQuality->SetDataDisplayName(FText::FromString(TEXT("后处理质量")));
			PostProcessingQuality->SetDescriptionRichText(GET_DESCRIPTION("PostProcessingQualityDescKey"));
			PostProcessingQuality->AddIntegerOption(0, FText::FromString(TEXT("低")));
			PostProcessingQuality->AddIntegerOption(1, FText::FromString(TEXT("一般")));
			PostProcessingQuality->AddIntegerOption(2, FText::FromString(TEXT("高")));
			PostProcessingQuality->AddIntegerOption(3, FText::FromString(TEXT("史诗")));
			PostProcessingQuality->AddIntegerOption(4, FText::FromString(TEXT("电影级")));
			PostProcessingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
			PostProcessingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));
			PostProcessingQuality->SetShouldApplySettingsImmediately(true);
			// 两个相互依赖 如果单独设置全局光照，则全局质量变为自定义
			PostProcessingQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(PostProcessingQuality);
			GraphicsCategoryCollection->AddChildListData(PostProcessingQuality);
		}
		
	}
	// 高级图像类别
	{
		UListDataObject_Collection* AdvancedGraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
		AdvancedGraphicsCategoryCollection->SetDataID(FName("AdvancedGraphicsCategoryCollection"));
		AdvancedGraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("高级图形")));
		VideoTabCollection->AddChildListData(AdvancedGraphicsCategoryCollection);

		//垂直同步
		{
			UListDataObject_StringBool* VerticalSync = NewObject<UListDataObject_StringBool>();
			VerticalSync->SetDataID(FName("VerticalSync"));
			VerticalSync->SetDataDisplayName(FText::FromString(TEXT("垂直同步")));
			VerticalSync->SetDescriptionRichText(GET_DESCRIPTION("VerticalSyncDescKey"));
			VerticalSync->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
			VerticalSync->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));
			VerticalSync->SetFalseAsDefaultValue();
			VerticalSync->SetShouldApplySettingsImmediately(true);

			// 该选项在编辑器中不可编辑 只有打包后才能编辑
			FOptionsDataEditConditionDescriptor FullscreenOnlyCondition;
			FullscreenOnlyCondition.SetEditConditionFunc(
				[CreatedWindowMode]()->bool
			{
				return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
			});
#if WITH_EDITOR	
			FullscreenOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>只有在打包后Fullscreen下才能编辑</>"));
#endif
			FullscreenOnlyCondition.SetDisabledForcedStringValue(TEXT("false"));

			VerticalSync->AddEditCondition(FullscreenOnlyCondition);
			AdvancedGraphicsCategoryCollection->AddChildListData(VerticalSync);
		}
		// 帧率限制
		{
			UListDataObject_String* FrameRateLimit = NewObject<UListDataObject_String>();
			FrameRateLimit->SetDataID(FName("FrameRateLimit"));
			FrameRateLimit->SetDataDisplayName(FText::FromString(TEXT("帧率限制")));
			FrameRateLimit->SetDescriptionRichText(GET_DESCRIPTION("FrameRateLimitDescKey"));
			FrameRateLimit->AddDynamicOption(LexToString(30.f), FText::FromString(TEXT("30 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(60.f), FText::FromString(TEXT("60 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(90.f), FText::FromString(TEXT("90 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(0.f), FText::FromString(TEXT("无限制")));
			FrameRateLimit->SetDefaultValueFromString(LexToString(0.f));
			FrameRateLimit->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
			FrameRateLimit->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));
			FrameRateLimit->SetShouldApplySettingsImmediately(true);

			AdvancedGraphicsCategoryCollection->AddChildListData(FrameRateLimit);
		}
	}
	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("控制")));
	//TODO: 暂时没有学到不管

	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData,
	TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	// 检查父数据对象是否有效，以及是否包含子数据
	if (!InParentData || !InParentData->HasAnyChildListData()) return;

	// 遍历父对象的所有子数据项
	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		// 跳过空的子数据项
		if (!SubChildListData) continue;

		// 将有效的子数据项添加到输出数组中
		OutFoundChildListData.Add(SubChildListData);
		
		// 如果子数据项还包含更深层的子数据，则递归调用此函数继续查找
		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
}

