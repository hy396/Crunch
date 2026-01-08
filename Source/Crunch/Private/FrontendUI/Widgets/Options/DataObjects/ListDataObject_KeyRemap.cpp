// 幻雨喜欢小猫咪


#include "ListDataObject_KeyRemap.h"

#include "CommonInputSubsystem.h"
#include "HuanYuDebugHelper.h"

void UListDataObject_KeyRemap::InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings,
                                                UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType,
                                                const FPlayerKeyMapping& InOwningPlayerKeyMapping)
{
	// 缓存输入用户设置对象
	CachedOwningInputUserSettings = InOwningInputUserSettings;
	
	// 缓存键位配置文件
	CachedOwningKeyProfile = InKeyProfile;
	
	// 缓存期望的输入设备类型
	CachedDesiredInputKeyType = InDesiredInputKeyType;
	
	// 缓存映射名称
	CachedOwningMappingName = InOwningPlayerKeyMapping.GetMappingName();
	
	// 缓存可映射键位槽
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();
}

FSlateBrush UListDataObject_KeyRemap::GetIconFromCurrentKey() const
{
	// 检查缓存的输入用户设置对象是否有效
	check(CachedOwningInputUserSettings);
	
	// 创建一个Slate画刷对象
	FSlateBrush FoundBrush;
	
	// 获取通用输入子系统
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedOwningInputUserSettings->GetLocalPlayer());
	check(CommonInputSubsystem);

	// 尝试获取输入画刷
	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		FoundBrush, GetOwningKeyMapping()->GetCurrentKey(),
		CachedDesiredInputKeyType, CommonInputSubsystem->GetCurrentGamepadName());

	// 如果没有找到对应画刷，则输出调试信息
	if (!bHasFoundBrush)
	{
		Debug::Print(TEXT("根据这个key：") + GetOwningKeyMapping()->GetCurrentKey().GetDisplayName().ToString()+
			TEXT("找不到对应的图标，该图标没有设置为空"));
	}
	
	// 返回找到的画刷
	return FoundBrush;
}

void UListDataObject_KeyRemap::BindNewInputKey(const FKey& InNewKey)
{
	// 检查缓存的输入用户设置对象是否有效
	check(CachedOwningInputUserSettings);
	
	// 创建映射玩家键位参数结构体
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	KeyArgs.NewKey = InNewKey;

	// 创建游戏标签容器
	FGameplayTagContainer Container;
	
	// 映射玩家键位
	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs, Container);
	
	// 保存设置
	CachedOwningInputUserSettings->SaveSettings();

	// 通知列表数据已修改
	NotifyListDataModified(this);
}

bool UListDataObject_KeyRemap::HasDefaultValue() const
{
	// 检查拥有的键位映射的默认键是否有效
	return GetOwningKeyMapping()->GetDefaultKey().IsValid();
}

bool UListDataObject_KeyRemap::CanResetBackToDefaultValue() const
{
	// 检查是否有默认值且键位映射已被自定义
	return HasDefaultValue() && GetOwningKeyMapping()->IsCustomized();
}

bool UListDataObject_KeyRemap::TryResetBackToDefaultValue()
{
	// 检查是否可以重置回默认值
	if (CanResetBackToDefaultValue())
	{
		// 检查缓存的输入用户设置对象是否有效
		check(CachedOwningInputUserSettings);
		
		// 重置键位映射到默认值
		GetOwningKeyMapping()->ResetToDefault();
		
		// 保存设置
		CachedOwningInputUserSettings->SaveSettings();
		
		// 通知列表数据已修改，原因是重置为默认值
		NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
		
		// 返回成功
		return true;
	}
	
	// 返回失败
	return false;
}

FPlayerKeyMapping* UListDataObject_KeyRemap::GetOwningKeyMapping() const
{
	// 检查缓存的键位配置文件是否有效
	check(CachedOwningKeyProfile);
	
	// 创建映射玩家键位参数结构体
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	
	// 查找并返回键位映射
	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs);
}
