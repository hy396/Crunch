// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "ListDataObject_Collection.generated.h"

/**
 * 列表数据集合类
 * 用于管理和组织多个子列表数据对象的容器类
 */
UCLASS()
class CRUNCH_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()
public:
	// begin UListDataObject_Base 
	// 获取所有子列表数据对象
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const override;
	
	// 检查是否存在任何子列表数据对象
	virtual bool HasAnyChildListData() const override;
	// end UListDataObject_Base 

	// 添加子列表数据对象
	void AddChildListData(UListDataObject_Base* InChildListData);
	
private:
	// 子列表数据对象数组
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ChildListDataArray;
};
