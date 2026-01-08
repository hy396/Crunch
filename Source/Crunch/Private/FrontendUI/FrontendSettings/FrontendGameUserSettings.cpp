// 幻雨喜欢小猫咪


#include "FrontendGameUserSettings.h"

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
