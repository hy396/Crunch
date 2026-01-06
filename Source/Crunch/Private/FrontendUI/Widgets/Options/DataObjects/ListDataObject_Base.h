// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "FrontendUI/FrontendTypes/FrontendEnumTypes.h"
#include "FrontendUI/FrontendTypes/FrontendStructTypes.h"
#include "UObject/Object.h"
#include "ListDataObject_Base.generated.h"


// ACCESSOR 辅助宏：为属性生成getter和setter方法
#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const { return PropertyName;} \
	void Set##PropertyName(DataType In##PropertyName) {PropertyName = In##PropertyName;}

/**
 * 列表数据对象基类
 * 这是一个抽象类，用于定义选项系统中各种数据对象的基础接口和通用功能
 */
UCLASS(Abstract)  // 不能被实例化 蓝图中看不到
class CRUNCH_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()
public:
	// 声明一个多播委托，当列表数据修改时触发，参数为修改的数据对象和修改原因
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, UListDataObject_Base*, EOptionsListDataModifyReason);
	
	// 数据修改时触发的委托
	FOnListDataModifiedDelegate OnListDataModified;
	
	// 依赖数据修改时触发的委托
	FOnListDataModifiedDelegate OnDependencyDataModified;

	
	// 为各种基础属性生成访问器方法
	LIST_DATA_ACCESSOR(FName, DataID)
	LIST_DATA_ACCESSOR(FText, DataDisplayName)
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)
	LIST_DATA_ACCESSOR(FText, DisabledRichText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
	LIST_DATA_ACCESSOR(UListDataObject_Base*, ParentData)
	
	// 获取所有子列表数据
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const {return TArray<UListDataObject_Base*>();}
	
	// 检查是否存在任何子列表数据
	virtual bool HasAnyChildListData() const {return false;}

	// 初始化数据对象
	void InitDataObject();

	// 设置是否应立即应用设置更改
	void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway){bShouldApplyChangeImmediately=bShouldApplyRightAway;}

	// 检查数据对象是否具有默认值
	virtual bool HasDefaultValue() const {return false;}
	
	// 检查数据对象是否可以重置为默认值
	virtual bool CanResetBackToDefaultValue() const {return false;}
	
	// 尝试将数据对象重置为默认值
	virtual bool TryResetBackToDefaultValue() {return false;}

	// 添加编辑条件描述符(当从选项调用时，这个函数会被调用，添加到注册中，在条件构建时作为数据对象)
	void AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);
	
	// 检查数据当前是否可编辑
	bool IsDataCurrentlyEditable();

	// 添加依赖数据对象(从选项调用注册以添加依赖数据)
	void AddEditDependencyData(UListDataObject_Base* InDependencyData);

protected:
	// 数据对象初始化完成时调用(子类需要重写该函数 父类中是空的)
	virtual void OnDataObjectInitialized();

	// 通知列表数据已修改(用户设置改变时代理进行通知)
	virtual void NotifyListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);

	// 检查是否可以设置为强制字符串值
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const { return false; }

	// 当设置为强制字符串值时调用
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) {}

	// 依赖代理回调函数 当依赖数据的值发生变化时，子类可以重写此函数来处理所需的自定义逻辑，在父版本中只广播依赖数据修改的委托
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason);
	
private:
	// 数据标识符
	FName DataID;
	
	// 数据显示名称
	FText DataDisplayName;
	
	// 描述富文本
	FText DescriptionRichText;
	
	// 禁用状态下的富文本
	FText DisabledRichText;

	// 描述图片的软引用
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;

	// 父级数据对象指针
	UPROPERTY(Transient)
	UListDataObject_Base* ParentData; // 在某个标签下的数据，标签按钮的数据将是他们的父项

	// 是否应立即应用更改
	bool bShouldApplyChangeImmediately = false;

	// 编辑条件描述符数组
	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
	
};
