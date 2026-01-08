// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "FrontendCommonRotator.generated.h"

/**
 * 一个可以在给定文本标签之间旋转的按钮。
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UFrontendCommonRotator : public UCommonRotator
{
	GENERATED_BODY()
public:
	// 设置当前选项的文本
	void SetSelectedOptionByText(const FText& InTextOption);

};
