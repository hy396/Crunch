// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "StatusEffectWidget.generated.h"

class UStatusEffectItemWidget;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FStatusEffectData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 状态效果标签
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag StatusEffectTag;

	// 状态效果图标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	// 状态效果名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	// 状态效果描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

/**
 * 状态效果系统UI控件，用于显示角色当前受到的所有状态效果（包括正面和负面效果）
 */
UCLASS()
class CRUNCH_API UStatusEffectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 配置状态效果显示组件
	void ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent);

	// 设置状态效果数据表
	void SetStatusEffectDataTable(UDataTable* DataTable) { StatusEffectDataTable = DataTable; }

	// 获取状态效果数据表
	UDataTable* GetStatusEffectDataTable() const { return StatusEffectDataTable; }

	// 获取OwnerASC
	UAbilitySystemComponent* GetOwnerASC() const { return OwnerAbilitySystemComponent; }

// private:
	// 包装布局容器，用于显示所有状态效果项目（支持自动换行）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> StatusEffectContainer;

	// 能力系统组件引用
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;

	// 当前激活的状态效果句柄
	TArray<FActiveGameplayEffectHandle> ActiveStatusEffectHandles;

	// 状态效果项目控件类
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect")
	TSubclassOf<UStatusEffectItemWidget> StatusEffectItemWidgetClass;

	// 状态效果数据表
	UPROPERTY(EditDefaultsOnly, Category = "StatusEffect")
	TObjectPtr<UDataTable> StatusEffectDataTable;

	void InitStatusEffectData();
	UFUNCTION(BlueprintCallable)
	void UpdateStatusEffectItem();

	// 当前该函数可以移除了, 已经在状态效果项目类中实现了自动移除的效果
	UFUNCTION(BlueprintCallable)
	void RemoveExpiredStatusEffectItems();
	// 移除状态效果项目
	void RemoveStatusEffectItem(FActiveGameplayEffectHandle Handle);
	// 状态效果数据表, 通过标签查找
	TMap<FGameplayTag, FStatusEffectData> StatusEffectDataMap;

	// 对应Tag的UI项
	TMap<FGameplayTag, UStatusEffectItemWidget*> StatusEffectTagMap;
	TMap<FActiveGameplayEffectHandle, UStatusEffectItemWidget*> EffectHandleToWidget;

};