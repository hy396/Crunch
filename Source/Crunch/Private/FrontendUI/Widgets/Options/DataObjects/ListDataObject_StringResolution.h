// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_String.h"
#include "ListDataObject_StringResolution.generated.h"

/**
 * 字符串分辨率列表数据对象类
 * 专门用于处理屏幕分辨率选项的特殊字符串列表数据对象
 */
UCLASS()    // Resolution 屏幕清晰度
class CRUNCH_API UListDataObject_StringResolution : public UListDataObject_String
{
	GENERATED_BODY()
public:
	// 初始化分辨率值
	void InitResolutionValues();

	// 获取最大允许的分辨率
	FORCEINLINE FString GetMaximumAllowedResolution() const{return MaximumAllowedResolution;}
	
protected:
	// ~begin UListDataObject_String 接口
	// 数据对象初始化时调用
	virtual void OnDataObjectInitialized() override;
	// ~end UListDataObject_String 接口


private:
	// 将分辨率转换为值字符串
	FString ResToValueString(const FIntPoint& InResolution) const;
	
	// 将分辨率转换为显示文本
	FText ResToDisplayText(const FIntPoint& InResolution) const;

	// 最大允许分辨率
	FString MaximumAllowedResolution;
};
