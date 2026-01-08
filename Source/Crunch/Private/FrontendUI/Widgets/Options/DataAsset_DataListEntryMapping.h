// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_DataListEntryMapping.generated.h"

class UWidget_ListEntry_Base;     // 选项列表条目 Widget 基类
class UListDataObject_Base;       // 选项列表数据对象基类

/**
 * 数据对象 ↔ 列表条目控件 映射表（Data Asset）
 *
 * 作用：
 * 在选项界面中，不同类型的设置项（例如字符串选择、滑块、布尔开关、整数输入等）
 * 需要使用不同的列表条目 Widget 来显示和交互。
 *
 * 本 Data Asset 提供了一个可编辑的映射表：
 *   数据对象类（UListDataObject_XXX） → 对应的条目控件类（UWidget_ListEntry_XXX）
 *
 * 通过这个映射，选项系统可以在运行时动态为每一种数据对象创建正确的 UI 控件，
 * 实现高度可扩展的选项界面，而无需硬编码大量 if-else 判断。
 *
 * 使用方式：
 * 在项目设置或蓝图中创建一个此 Data Asset 的实例，填充映射表，然后在选项列表控件中引用它。
 */
UCLASS()
class CRUNCH_API UDataAsset_DataListEntryMapping : public UDataAsset
{
	GENERATED_BODY()
public:
	/**
	 * 根据给定的数据对象实例，查找并返回对应的列表条目 Widget 类
	 * 支持继承：会沿着类层次向上查找，直到找到匹配的映射或到达基类
	 *
	 * @param InDataObject  运行时的选项数据对象实例（不能为空）
	 * @return              对应的 UWidget_ListEntry_Base 子类，如果未找到返回 nullptr
	 */
	TSubclassOf<UWidget_ListEntry_Base> FindEntryWidgetClassByDataObject(UListDataObject_Base* InDataObject) const;

private:
	/**
	 * 编辑器可见的映射表
	 * Key   : 数据对象类（例如 UListDataObject_String、UListDataObject_Slider 等）
	 * Value : 对应的列表条目控件类（例如 UWidget_ListEntry_String、UWidget_ListEntry_Slider 等）
	 *
	 * 支持子类映射：如果某个具体子类没有配置，会自动回退查找其父类映射
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Options Entry Mapping")
	TMap<TSubclassOf<UListDataObject_Base>, TSubclassOf<UWidget_ListEntry_Base>> DataObjectListEntryMap;
};
