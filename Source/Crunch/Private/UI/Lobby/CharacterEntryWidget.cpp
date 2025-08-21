// 幻雨喜欢小猫咪


#include "CharacterEntryWidget.h"

#include "Character/PDA_CharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CharacterDefinition = Cast<UPDA_CharacterDefinition>(ListItemObject);
	if (CharacterDefinition)
	{
		// 设置图标
		CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureMatParamName, CharacterDefinition->LoadIcon());
		// 设置名称
		CharacterNameText->SetText(FText::FromString(CharacterDefinition->GetCharacterDisplayName()));
	}
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected)
{
	CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, bIsSelected ? 0.f : 1.f);
}
