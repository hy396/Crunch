// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TreeNodeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTreeNodeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 树节点接口，用于实现树形数据结构中的节点功能
 * 适用于技能树、科技树、组织结构图等需要节点关系的系统
 */
class ITreeNodeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 获取该节点关联的UI控件
	 * @return 返回节点对应的用户控件实例
	 */
	virtual UUserWidget* GetWidget() const = 0;

	/**
	 * 获取当前节点的所有输入节点（父节点/前置节点）
	 * @return 输入节点接口指针的数组
	 */
	virtual TArray<const ITreeNodeInterface*> GetInputs() const = 0;

	/**
	 * 获取当前节点的所有输出节点（子节点/后置节点）
	 * @return 输出节点接口指针的数组
	 */
	virtual TArray<const ITreeNodeInterface*> GetOutputs() const = 0;

	/**
	 * 获取节点关联的数据对象
	 * @return 节点关联的UObject数据对象（如技能配置、科技配置等）
	 */
	virtual const UObject* GetItemObject() const = 0;	
};
