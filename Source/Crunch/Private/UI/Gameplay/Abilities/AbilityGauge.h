// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilityGauge.generated.h"

// 技能控件数据结构，用于配置技能UI显示内容
USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	// 技能类
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass;

	// 技能名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	// 技能图标
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	// 技能描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

/**
 * 技能槽UI控件，负责显示技能图标、冷却、等级、消耗等信息
 * 支持技能升级、冷却计时、能否释放等状态的动态刷新
 */
UCLASS()
class CRUNCH_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	// 控件构建时调用
	virtual void NativeConstruct() override;

	// 列表项对象设置时调用
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// 用数据配置控件显示
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);
private:
	// 冷却倒数计时的间隔
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	// 图标材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";

	// 冷却百分比材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CooldownPercentParamName = "Percent";

	// 技能等级材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName AbilityLevelParamName = "Level";

	// 最大升级的材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName MaxLevelParamName = "UTiling";

	// 能否释放技能材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CanCastAbilityParamName = "CanCast";
    
	// 是否有可用升级点材质参数名
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName UpgradePointAvailableParamName = "UpgradeAvailable";
	
	// 技能图标
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	// 技能等级进度条控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> LevelGauge;
	
	// 冷却计时文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownCounterText;

	// 冷却总时长文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CooldownDurationText;

	// 技能消耗文本控件
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CostText;

	// 技能类默认对象
	UPROPERTY()
	TObjectPtr<UGameplayAbility> AbilityCDO;

	// 技能释放时回调
	void AbilityCommitted(UGameplayAbility* Ability);

	// 启动冷却计时
	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);

	// 缓存的冷却总时长
	float CachedCooldownDuration;

	// 缓存的冷却剩余时间
	float CachedCooldownTimeRemaining;

	// 冷却完成定时器
	FTimerHandle CooldownTimerHandle;

	// 冷却刷新定时器
	FTimerHandle CooldownTimerUpdateHandle;

	// 数字格式化选项（整数）
	FNumberFormattingOptions WholeNumberFormattingOptions;

	// 数字格式化选项（一位小数）
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;

	// 冷却完成回调，隐藏冷却文本并重置进度条
	void CooldownFinished();

	// 冷却刷新回调，更新冷却文本和进度条
	void UpdateCooldown();

	// 技能所属的能力组件
	UPROPERTY()
	TObjectPtr<const UAbilitySystemComponent> OwnerAbilitySystemComponent;

	// 缓存的技能句柄
	FGameplayAbilitySpecHandle CachedAbilitySpecHandle;

	// 获取技能Spec
	const FGameplayAbilitySpec* GetAbilitySpec();

	// 技能是否学习
	bool bIsAbilityLearned = false;

	// 当技能规格更新时刷新UI显示（等级/冷却/消耗等）
	void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);

	// 更新技能能否释放的状态（考虑法力/学习状态等）
	void UpdateCanCast();

	// 当升级点数变化时刷新升级提示
	void UpgradePointUpdated(const FOnAttributeChangeData& Data);

	// 法力变化回调
	void ManaUpdated(const FOnAttributeChangeData& Data);
};
