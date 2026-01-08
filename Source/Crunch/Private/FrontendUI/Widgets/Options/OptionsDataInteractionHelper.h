// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"

class UFrontendGameUserSettings;

/**
 * FOptionsDataInteractionHelper
 *
 * 作用：
 * 1. 提供一个通用接口，通过“函数路径”动态读取和写入 UFrontendGameUserSettings 的属性值
 * 2. 支持把属性值作为 FString 获取或设置，便于绑定 UI（Slider/TextBox/Dropdown 等）
 *
 * 使用场景：
 * - 前端设置界面 Widget 需要动态绑定到不同设置字段
 * - 不想手动写每个 Get/Set 的代码
 *
 * 核心思想：
 * - FCachedPropertyPath 保存了 Getter 或 Setter 的函数路径
 * - 内部持有 UFrontendGameUserSettings 的弱引用（TWeakObjectPtr）避免生命周期问题
 *
 * 注意事项：
 * - 构造函数中会获取单例 UFrontendGameUserSettings，如果还没初始化可能返回 nullptr
 * - Set / Get 都是基于 PropertyPathHelpers 的反射机制
 */
class CRUNCH_API FOptionsDataInteractionHelper
{
public:
	/**
	 * 构造函数
	 * @param InSetterOrGetterFuncPath - Getter 或 Setter 函数路径（可以是属性名或函数名）
	 * 例如：TEXT("OverallVolume") 或 TEXT("SetOverallVolume")
	 */
	FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath);

	/**
	 * 获取属性值并转换为 FString
	 * @return 属性值的字符串表示
	 * 注意：返回值只读，直接操作需要调用 SetValueFromString
	 */
	FString GetValueAsString() const;

	/**
	 * 从字符串设置属性值
	 * @param InValueString - 字符串表示的值
	 * 内部会根据属性类型自动转换
	 */
	void SetValueFromString(const FString& InValueString);

private:
	/** 
	 * 缓存的属性/函数路径
	 * - 通过 PropertyPathHelpers 解析并调用对应的 Getter/Setter
	 */
	FCachedPropertyPath CachedDynamicFunctionPath;

	/** 
	 * 弱引用到 UFrontendGameUserSettings 单例
	 * - 使用弱引用避免持有生命周期，防止悬挂指针
	 */
	TWeakObjectPtr<UFrontendGameUserSettings> CachedWeakGameUserSettings;
	
};
