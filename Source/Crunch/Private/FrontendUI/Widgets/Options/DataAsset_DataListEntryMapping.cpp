// 幻雨喜欢小猫咪


#include "DataAsset_DataListEntryMapping.h"

#include "DataObjects/ListDataObject_Base.h"

/**
 * 根据数据对象实例查找对应的列表条目 Widget 类
 * 实现原理：从具体类开始，逐级向上遍历继承链，查找映射表中是否存在匹配的键
 *
 * 这样设计的好处：
 * - 支持子类复用父类的条目控件（例如所有字符串类型子类都可以共用同一个 String Entry）
 * - 无需为每一个具体数据类都配置映射，减少配置工作量
 *
 * @param InDataObject  传入的选项数据对象实例
 * @return              找到的条目控件类（TSubclassOf），未找到返回空
 */
TSubclassOf<UWidget_ListEntry_Base> UDataAsset_DataListEntryMapping::FindEntryWidgetClassByDataObject(
	UListDataObject_Base* InDataObject) const
{
	// 安全检查：确保传入的对象有效（开发阶段捕获潜在错误）
	check(InDataObject);

	// 从当前具体类开始，逐级向上遍历继承链（包括自身）
	for (UClass* DataObjectClass = InDataObject->GetClass(); DataObjectClass; DataObjectClass = DataObjectClass->GetSuperClass())
	{
		// 将 UClass* 转换为 TSubclassOf<UListDataObject_Base> 类型（安全转换）
		TSubclassOf<UListDataObject_Base> ConvertedDataObjectClass = DataObjectClass;

		// 检查映射表中是否包含该类（或其父类）的映射
		if (DataObjectListEntryMap.Contains(ConvertedDataObjectClass))
		{
			// 找到匹配项，直接返回对应的条目控件类
			return DataObjectListEntryMap.FindRef(ConvertedDataObjectClass);
		}
	}

	// 遍历完整个继承链仍未找到匹配的映射，返回空（调用方需处理默认情况）
	return TSubclassOf<UWidget_ListEntry_Base>();
}
