// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "PDA_CharacterDefinition.generated.h"

class ACCharacter;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class CRUNCH_API UPDA_CharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 获取当前数据资产的唯一标识符
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	// 获取角色定义资产类型（用于资产管理）
	static FPrimaryAssetType GetCharacterDefinitionAssetType();

	// 获取角色显示名称
	FString GetCharacterDisplayName() const { return CharacterName; }
	// 加载角色图标纹理
	UTexture2D* LoadIcon() const;
	// 加载角色蓝图类
	TSubclassOf<ACCharacter> LoadCharacterClass() const;
	// 加载显示用的动画蓝图
	TSubclassOf<UAnimInstance> LoadDisplayAnimationBP() const;
	// 加载显示用的骨骼网格
	class USkeletalMesh* LoadDisplayMesh() const;
	// 加载角色入场动画
	UAnimMontage* LoadCharacterIntroAnimation() const;
	// 获取能力映射表（输入ID到技能类的映射）
	const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>* GetAbilities() const;

private:	
	// 角色显示名称
	UPROPERTY(EditDefaultsOnly, Category = "Character", meta = (DisplayName = "角色名称"))
	FString CharacterName;

	// 角色图标纹理
	UPROPERTY(EditDefaultsOnly, Category = "Character", meta = (DisplayName = "角色图像"))
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	// 角色入场动画
	UPROPERTY(EditDefaultsOnly, Category = "Character", meta = (DisplayName = "入场动画"))
	TSoftObjectPtr<UAnimMontage> CharacterIntroAnimation;

	// 角色蓝图类
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<ACCharacter> CharacterClass;

	// 显示用的动画蓝图
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<UAnimInstance> DisplayAnimBP;
};
