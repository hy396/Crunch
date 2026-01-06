// 幻雨喜欢小猫咪


#include "ListDataObject_Collection.h"

TArray<UListDataObject_Base*> UListDataObject_Collection::GetAllChildListData() const
{
	return ChildListDataArray;
}

bool UListDataObject_Collection::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}

void UListDataObject_Collection::AddChildListData(UListDataObject_Base* InChildListData)
{
	// 初始化子数据对象
	InChildListData->InitDataObject();
	
	// 设置子列表数据的父对象为当前集合对象
	InChildListData->SetParentData(this);
	
	// 将子数据对象添加到数组中
	ChildListDataArray.Add(InChildListData);
}
