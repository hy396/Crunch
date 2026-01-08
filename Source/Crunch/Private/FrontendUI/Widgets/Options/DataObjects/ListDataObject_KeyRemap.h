// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "ListDataObject_KeyRemap.generated.h"

/**
 * 键位重映射列表数据对象类
 * 用于管理游戏中键位设置的数据表示和操作逻辑
 */
UCLASS()
class CRUNCH_API UListDataObject_KeyRemap : public UListDataObject_Base
{
	GENERATED_BODY()
public:
	/**
	 * 初始化键位重映射数据
	 * @param InOwningInputUserSettings 拥有的输入用户设置对象
	 * @param InKeyProfile 键位配置文件
	 * @param InDesiredInputKeyType 期望的输入设备类型
	 * @param InOwningPlayerKeyMapping 玩家键位映射信息
	 */
	void InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* InKeyProfile,
		ECommonInputType InDesiredInputKeyType, const FPlayerKeyMapping& InOwningPlayerKeyMapping);

	/**
	 * 根据当前键位获取图标
	 * @return 当前键位对应的图标
	 */
	FSlateBrush GetIconFromCurrentKey() const;

	/**
	 * 绑定新输入键位
	 * @param InNewKey 新的键位
	 */
	void BindNewInputKey(const FKey& InNewKey);

	
	// ~begin UListDataObject_Base 
	virtual bool HasDefaultValue() const override;
	virtual bool CanResetBackToDefaultValue() const  override;
	virtual bool TryResetBackToDefaultValue()  override;
	// ~end UListDataObject_Base

private:
	// 缓存的输入用户设置对象
	UPROPERTY(Transient)
	UEnhancedInputUserSettings* CachedOwningInputUserSettings;

	// 缓存的键位配置文件
	UPROPERTY(Transient)
	UEnhancedPlayerMappableKeyProfile* CachedOwningKeyProfile;

	// 缓存的期望输入设备类型
	ECommonInputType CachedDesiredInputKeyType;

	// 缓存的映射名称
	FName CachedOwningMappingName;

	// 缓存的可映射键位槽
	EPlayerMappableKeySlot CachedOwningMappableKeySlot;

	/**
	 * 获取拥有的键位映射对象
	 * @return 键位映射对象指针
	 */
	FPlayerKeyMapping* GetOwningKeyMapping() const;
	
public:
	/**
	 * 获取期望的输入设备类型
	 * @return 输入设备类型
	 */
	FORCEINLINE ECommonInputType GetDesiredInputKeyType() const { return CachedDesiredInputKeyType; }
};
