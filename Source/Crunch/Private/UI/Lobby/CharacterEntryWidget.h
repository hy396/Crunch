// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CharacterEntryWidget.generated.h"

class UTextBlock;
class UImage;
class UPDA_CharacterDefinition;
/**
 * 角色列表条目控件
 * 用于在角色选择界面中显示单个角色的图标和名称
 */
UCLASS()
class CRUNCH_API UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	// 当列表项绑定数据对象时调用
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// 获取当前绑定的角色定义数据
	FORCEINLINE const UPDA_CharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
	
	// 设置条目选中状态
	void SetSelected(bool bIsSelected);
private:	
	// 角色图标控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> CharacterIcon;

	// 角色名称文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CharacterNameText;

	// 材质参数名称：图标纹理参数（用于动态材质调整）
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName IconTextureMatParamName = "Icon";

	// 材质参数名称：饱和度参数（用于选中状态高亮）
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName SaturationMatParamName = "Saturation";

	// 当前绑定的角色定义数据
	UPROPERTY()
	const UPDA_CharacterDefinition* CharacterDefinition;
};
