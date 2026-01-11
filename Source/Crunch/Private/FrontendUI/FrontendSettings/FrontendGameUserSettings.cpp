// 幻雨喜欢小猫咪


#include "FrontendGameUserSettings.h"
#include "FrontendUI/FrontendFunctionLibrary.h"
#include "FrontendUI/Core/FrontendGameplayTags.h"
#include "Kismet/GameplayStatics.h"

/**
 * 构造函数
 *
 * 初始化默认设置值：
 * - 这些值只在首次生成配置文件时生效
 * - 如果 ini 中已有值，则会被 Config 系统覆盖
 */
UFrontendGameUserSettings::UFrontendGameUserSettings()
	: OverallVolume(1.f)
	, MusicVolume(1.f)
	, SoundFXVolume(1.f)
	, bAllowBackgroundAudio(false)
	, bUseHDRAudioMode(false)
{
}

/**
 * 获取自定义的 GameUserSettings 单例
 *
 * 使用场景：
 * - UI Widget 中读取或修改设置
 * - Frontend Subsystem 同步用户配置
 */
UFrontendGameUserSettings* UFrontendGameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

/**
 * 获取当前显示 Gamma
 *
 * Gamma 值存储在 GEngine 中，而不是 GameUserSettings
 */
float UFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}
	return 0.f;
}

/**
 * 设置显示 Gamma
 *
 * 注意：
 * - 该设置是立即生效的
 * - 不会自动保存到配置文件
 */
void UFrontendGameUserSettings::SetCurrentDisplayGamma(float InNewValue)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InNewValue;
	}
}

/**
 * 重写 ApplySettings
 * 在基类应用设置后，额外应用音频设置
 */
void UFrontendGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	// 先调用基类的 ApplySettings 应用其他设置（视频、游戏等）
	Super::ApplySettings(bCheckForCommandLineOverrides);

	// 应用音频设置
	ApplyAudioSettings();
}

void UFrontendGameUserSettings::ApplyAudioSettings()
{
	// 获取当前世界上下文
	UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("应用音频设置：无效的世界上下文"));
		return;
	}

	// 检查缓存，如果未缓存则加载
	if (!CachedMasterBusMix)
	{
		TSoftObjectPtr<USoundMix> MasterMixSoft = UFrontendFunctionLibrary::GetMasterSoundMix();
		if (!MasterMixSoft.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("应用音频设置：主音频混音器未配置"));
			return;
		}
		CachedMasterBusMix = MasterMixSoft.LoadSynchronous();
		if (!CachedMasterBusMix)
		{
			UE_LOG(LogTemp, Warning, TEXT("应用音频设置：主音频混音器加载失败"));
			return;
		}
	}
	// 应用后台音频设置
	UGameplayStatics::SetAllowBackgroundAudio(World, bAllowBackgroundAudio);
	UE_LOG(LogTemp, Log, TEXT("后台音频设置：%s"), bAllowBackgroundAudio ? TEXT("启用") : TEXT("禁用"));

	// 应用 HDR 音频设置
	UGameplayStatics::SetUseHDRAudio(World, bUseHDRAudioMode);
	UE_LOG(LogTemp, Log, TEXT("HDR 音频设置：%s"), bUseHDRAudioMode ? TEXT("启用") : TEXT("禁用"));


	// 激活混音器
	UGameplayStatics::PushSoundMixModifier(World, CachedMasterBusMix);

	// 应用各个音量通道
	ApplyVolumeToSoundClass(World, FrontendGameplayTags::Audio::OverallVolume, OverallVolume, CachedMasterBusMix);
	ApplyVolumeToSoundClass(World, FrontendGameplayTags::Audio::MusicVolume, MusicVolume, CachedMasterBusMix);
	ApplyVolumeToSoundClass(World, FrontendGameplayTags::Audio::SoundFXVolume, SoundFXVolume, CachedMasterBusMix);
}

void UFrontendGameUserSettings::ApplyVolumeToSoundClass(UWorld* World, FGameplayTag InVolumeTag, float InVolume, USoundMix* InMix)
{
	if (!World || !InMix)
	{
		return;
	}

	// 检查缓存，如果未缓存则加载
	TObjectPtr<USoundClass>* CachedClassPtr = CachedVolumeBusMap.Find(InVolumeTag);
	if (!CachedClassPtr || !(*CachedClassPtr))
	{
		TSoftObjectPtr<USoundClass> SoundClassSoft = UFrontendFunctionLibrary::GetSoundClassByTag(InVolumeTag);
		if (!SoundClassSoft.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("应用音量到音频类别：未找到标签 %s 对应的音频类别"), *InVolumeTag.ToString());
			return;
		}

		USoundClass* SoundClass = SoundClassSoft.LoadSynchronous();
		if (!SoundClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("应用音量到音频类别：标签 %s 的音频类别加载失败"), *InVolumeTag.ToString());
			return;
		}

		CachedVolumeBusMap.Add(InVolumeTag, SoundClass);
		CachedClassPtr = CachedVolumeBusMap.Find(InVolumeTag);
	}

	// 设置 SoundMixClassOverride 来控制音量
	// 注意：将音量限制为最小 0.001，避免完全静音后声音停止播放无法恢复
	float ClampedVolume = FMath::Clamp(InVolume, 0.001f, 1.f);
	UGameplayStatics::SetSoundMixClassOverride(World, InMix, *CachedClassPtr, ClampedVolume);

	UE_LOG(LogTemp, Log, TEXT("已将音量 %.2f 应用到音频类别 %s"), InVolume, *(*CachedClassPtr)->GetName());
}
