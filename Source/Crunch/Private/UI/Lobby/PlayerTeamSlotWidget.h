// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UTextBlock;
class UImage;
class UPDA_CharacterDefinition;
/**
 * 队伍槽位UI控件 - 用于在队伍界面中显示单个玩家信息
 * 功能：
 *   - 显示玩家名称和选择的角色图标
 *   - 支持鼠标悬停效果（显示角色名称）
 *   - 动态更新玩家状态
 */
UCLASS()
class CRUNCH_API UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 控件初始化
	virtual void NativeConstruct() override;

	/**
	 * 更新槽位信息
	 * @param PlayerName 玩家名称
	 * @param CharacterDefinition 角色定义资产（包含图标和名称）
	 */
	void UpdateSlot(const FString& PlayerName, const UPDA_CharacterDefinition* CharacterDefinition);

	// 鼠标悬停事件处理
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	// 鼠标离开事件处理
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	// 悬停动画
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HoverAnim;

	// 角色图标
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerCharacterIcon;

	// 名称文本
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	// 材质参数：角色图标纹理
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterIconMatParamName = "Icon";

	// 材质参数：空槽位状态
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterEmptyMatParamName = "Empty";

	// 缓存的玩家名称
	FString CachedPlayerNameStr;
	
	// 缓存的角色名称
	FString CachedCharacterNameStr;

	// 更新名称文本显示（根据悬停状态）
	void UpdateNameText();
};
