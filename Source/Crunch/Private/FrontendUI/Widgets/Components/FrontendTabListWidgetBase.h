// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FrontendTabListWidgetBase.generated.h"

class UFrontendCommonButtonBase;

/**
 * 前端标签列表部件基类，用于管理标签页界面组件
 * 继承自UCommonTabListWidgetBase，提供标签页注册和管理功能
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class CRUNCH_API UFrontendTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
public:
	/**
	 * 请求注册一个新的选项标签页（Tab）
	 * @param InTabID 标签的唯一ID（FName类型，用于内部标识和查找）
	 * @param InTabDisplayName 标签按钮上要显示的文字（例如“图形设置”、“声音”等）
	 */
	void RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName);
	
private:
	/**
	 * 调试编辑器预览标签计数
	 * 在编辑器中预览时显示的标签数量，默认为3个
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Tab List Settings", meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax= "10"))
	int32 DebugEditorPreviewTabCount = 3;


	/**
	 * 标签按钮条目部件类
	 * 指定用于创建标签按钮的部件类
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Frontend Tab List Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFrontendCommonButtonBase> TabButtonEntryWidgetClass;  

	/**
	 * 验证编译默认值，确保TabButtonEntryWidgetClass不为空
	 * 防止忘记指定对应的类导致运行时错误
	 */
	//~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface
};
