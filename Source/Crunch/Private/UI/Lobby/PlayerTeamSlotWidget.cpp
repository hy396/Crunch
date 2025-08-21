// 幻雨喜欢小猫咪


#include "PlayerTeamSlotWidget.h"

#include "Character/PDA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerTeamSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 初始化：设置为空槽位状态
	PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 1);
	
	// 清空缓存的角色名称
	CachedCharacterNameStr = "";
}

void UPlayerTeamSlotWidget::UpdateSlot(const FString& PlayerName, const UPDA_CharacterDefinition* CharacterDefinition)
{
	// 缓存玩家名称
	CachedPlayerNameStr = PlayerName;

	if (CharacterDefinition)
	{
		// 设置角色图片
		PlayerCharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(
			CharacterIconMatParamName, CharacterDefinition->LoadIcon());
		// 设置角色为非空
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(
			CharacterEmptyMatParamName, 0);
		// 缓存角色名称
		CachedCharacterNameStr = CharacterDefinition->GetCharacterDisplayName();
	}else
	{
		// 无角色选择时：
		// 标记为空槽位
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 1);
		
		// 清空角色名称缓存
		CachedCharacterNameStr = "";
	}

	// 根据当前状态更新文本显示
	UpdateNameText();
}

void UPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	// 鼠标悬停时：
	// 1. 显示角色名称
	NameText->SetText(FText::FromString(CachedCharacterNameStr));
	
	// 2. 正向播放悬停动画
	PlayAnimationForward(HoverAnim);
}

void UPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// 鼠标离开时：
	// 1. 恢复显示玩家名称
	NameText->SetText(FText::FromString(CachedPlayerNameStr));
	
	// 2. 反向播放悬停动画（返回原始状态）
	PlayAnimationReverse(HoverAnim);
}

void UPlayerTeamSlotWidget::UpdateNameText()
{
	// 根据悬停状态决定显示内容
	if (IsHovered())
	{
		// 悬停时显示角色名称
		NameText->SetText(FText::FromString(CachedCharacterNameStr));
	}
	else
	{
		// 非悬停时显示玩家名称
		NameText->SetText(FText::FromString(CachedPlayerNameStr));
	}
}
