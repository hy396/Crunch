// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CommonNumericTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GAS/Core/CAttributeSet.h"
#include "AttributeWidget.generated.h"

/**
 * 角色属性数据结构，用于数据表中存储属性名称和值
 */
USTRUCT(BlueprintType)
struct FCharacterAttributeData : public FTableRowBase
{
	GENERATED_BODY()
	// 属性显示名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AttributeName;
	// 属性值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttribute AttributeValue;
};

/**
 * 
 */
UCLASS()
class CRUNCH_API UAttributeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	bool BindForAscToAttribute(UAbilitySystemComponent* AbilitySystemComponent, const FText& AttributeName, const FGameplayAttribute& Attribute);
	// 属性名称
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Hy_AttributeName;

	// 当前属性值
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonNumericTextBlock> AttributeValue;

	void SetAttributeValue(const FOnAttributeChangeData& ChangeData);
};
