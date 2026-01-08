// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FrontendGameUserSettings.generated.h"

/**
 * UFrontendGameUserSettings
 *
 * 自定义的 GameUserSettings 派生类，用于管理前端（Frontend）相关的用户设置数据。
 *
 * 该类的特点：
 * - 基于 UGameUserSettings，天然支持 Config 持久化（.ini 文件）
 * - 主要用于 UI（CommonUI / Frontend）层读取和写入设置
 * - 不直接负责应用效果（如音量、Gamma），而是作为“设置数据源”
 */
UCLASS()
class CRUNCH_API UFrontendGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
public:
	/** 构造函数：用于初始化默认设置值 */
	UFrontendGameUserSettings();


	/**
	 * 获取当前的 GameUserSettings 单例
	 *
	 * 注意：
	 * - GEngine->GetGameUserSettings() 返回的是 UGameUserSettings*
	 * - 这里强制 Cast 为 UFrontendGameUserSettings
	 * - 项目需在 DefaultEngine.ini 中配置使用该派生类
	 */
	static UFrontendGameUserSettings* Get();

	// ================= Gameplay Collection Tab =================
	// TODO: 这个东西只图一乐，结束就删
	/**
	 * 获取当前游戏难度（字符串形式）
	 *
	 * 设计说明：
	 * - 使用 FString 而非 enum，方便与 UI / DataTable / 本地化系统对接
	 * - 常用于“普通 / 困难 / 地狱”等配置项
	 */
	UFUNCTION()
	FString GetCurrentGameDifficulty() { return CurrentGameDifficulty; }

	/**
	 * 设置当前游戏难度
	 *
	 * 注意：
	 * - 这里只修改配置值，不直接影响游戏逻辑
	 * - 实际生效通常由 GameMode / Subsystem 监听或主动读取
	 */
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty)
	{
		CurrentGameDifficulty = InNewDifficulty;
	}

	// ================= Audio Collection Tab =================

	/** 获取整体音量（Master Volume） */
	UFUNCTION()
	float GetOverallVolume() const { return OverallVolume; }

	/** 设置整体音量 */
	UFUNCTION()
	void SetOverallVolume(float InNewVolume) { OverallVolume = InNewVolume; }

	/** 获取音乐音量 */
	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }

	/** 设置音乐音量 */
	UFUNCTION()
	void SetMusicVolume(float InNewVolume) { MusicVolume = InNewVolume; }

	/** 获取音效音量 */
	UFUNCTION()
	float GetSoundFXVolume() const { return SoundFXVolume; }

	/** 设置音效音量 */
	UFUNCTION()
	void SetSoundFXVolume(float InNewVolume) { SoundFXVolume = InNewVolume; }

	/** 是否允许后台播放音频（窗口失焦） */
	UFUNCTION()
	bool GetAllowBackgroundAudio() const { return bAllowBackgroundAudio; }

	/** 设置是否允许后台音频 */
	UFUNCTION()
	void SetAllowBackgroundAudio(bool bIsAllowed)
	{
		bAllowBackgroundAudio = bIsAllowed;
	}

	/** 是否启用 HDR 音频模式 */
	UFUNCTION()
	bool GetUseHDRAudioMode() const { return bUseHDRAudioMode; }

	/** 设置 HDR 音频模式 */
	UFUNCTION()
	void SetUseHDRAudioMode(bool bIsAllowed)
	{
		bUseHDRAudioMode = bIsAllowed;
	}

	// ================= Video Collection Tab =================

	/**
	 * 获取当前显示 Gamma 值
	 *
	 * Gamma 是一个全局渲染参数，实际存储在 GEngine 中
	 */
	UFUNCTION()
	float GetCurrentDisplayGamma() const;

	/**
	 * 设置显示 Gamma
	 *
	 * 注意：
	 * - 该函数会直接修改 GEngine->DisplayGamma
	 * - 不通过 Config 保存，需要额外调用 SaveSettings 才会持久化
	 */
	UFUNCTION()
	void SetCurrentDisplayGamma(float InNewValue);

private:
	// ================= Gameplay Collection Tab =================
	// TODO: 这个东西只图一乐，结束就删, 有个沟八的游戏难度
	/**
	 * 当前游戏难度
	 *
	 * UPROPERTY(Config)：
	 * - 会被自动序列化到 GameUserSettings.ini
	 * - 通过 LoadSettings / SaveSettings 进行读写
	 */
	UPROPERTY(Config)
	FString CurrentGameDifficulty;

	// ================= Audio Collection Tab =================

	/** 总音量（Master） */
	UPROPERTY(Config)
	float OverallVolume;

	/** 音乐音量 */
	UPROPERTY(Config)
	float MusicVolume;

	/** 音效音量 */
	UPROPERTY(Config)
	float SoundFXVolume;

	/** 是否允许后台音频播放 */
	UPROPERTY(Config)
	bool bAllowBackgroundAudio;

	/** 是否启用 HDR 音频 */
	UPROPERTY(Config)
	bool bUseHDRAudioMode;
};
