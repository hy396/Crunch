// 幻雨喜欢小猫咪


#include "PDA_CharacterDefinition.h"

#include "CCharacter.h"

FPrimaryAssetId UPDA_CharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UPDA_CharacterDefinition::GetCharacterDefinitionAssetType()
{
	return FPrimaryAssetType("CharacterDefinition");
}

UTexture2D* UPDA_CharacterDefinition::LoadIcon() const
{
	// 强制同步加载软引用资源
	return CharacterIcon.LoadSynchronous();
	// if (CharacterIcon.IsValid())
	// 	return CharacterIcon.Get();
	//
	// return nullptr;
}

TSubclassOf<ACCharacter> UPDA_CharacterDefinition::LoadCharacterClass() const
{
	return CharacterClass.LoadSynchronous();
	// if (CharacterClass.IsValid())
	// 	return CharacterClass.Get();
	//
	// return TSubclassOf<ACCharacter>();

}

TSubclassOf<UAnimInstance> UPDA_CharacterDefinition::LoadDisplayAnimationBP() const
{
	return DisplayAnimBP.LoadSynchronous();
	// if (DisplayAnimBP.IsValid())
	// 	return DisplayAnimBP.Get();
	//
	// return TSubclassOf<UAnimInstance>();
}

class USkeletalMesh* UPDA_CharacterDefinition::LoadDisplayMesh() const
{
	// 加载角色蓝图类
	TSubclassOf<ACCharacter> LoadedCharacterClass = LoadCharacterClass();
	if (!LoadedCharacterClass)
		return nullptr;

	// 获取默认角色实例（仅用于资产获取）
	ACharacter* Character = Cast<ACharacter>(LoadedCharacterClass.GetDefaultObject());
	if (!Character)
		return nullptr;

	// 提取骨骼网格资产
	return Character->GetMesh()->GetSkeletalMeshAsset();
}

//UAnimMontage* UPDA_CharacterDefinition::LoadCharacterIntroAnimation() const
//{
//	return CharacterIntroAnimation.LoadSynchronous();
//}

const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* UPDA_CharacterDefinition::GetAbilities() const
{
	// 加载角色蓝图类
	TSubclassOf<ACCharacter> LoadedCharacterClass = LoadCharacterClass();
	if (!LoadedCharacterClass)
		return nullptr;

	// 获取角色默认对象
	ACCharacter* Character = Cast<ACCharacter>(LoadedCharacterClass.GetDefaultObject());
	if (!Character)
		return nullptr;

	// 返回能力映射表指针（直接访问角色内部数据）
	return &(Character->GetAbilities());
}
