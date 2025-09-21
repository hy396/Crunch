// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "PlayerPortraitWidget.generated.h"

class UPDA_CharacterDefinition;
class UMaterialInstanceDynamic;
class UImage;
class UTextBlock;
class UPlayerStateMonitor;
class AMPlayerState;
class ACCharacter;
class UProgressBar; // 添加进度条类

// 添加属性变化数据的声明
struct FOnAttributeChangeData;

/**
 * 玩家头像控件
 * 用于显示玩家的英雄头像，并根据玩家状态（存活/死亡）切换亮显和变灰效果，并且拥有死亡倒数计时
 * 同时显示玩家血量条
 */
UCLASS()
class CRUNCH_API UPlayerPortraitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 控件初始化
	virtual void NativeConstruct() override;

	/**
	 * 设置玩家头像和能力系统组件
	 * 该函数用于初始化玩家头像控件，绑定能力系统组件的属性变化事件，
	 * 并根据角色定义设置头像图标。同时根据是否为友方单位设置不同的显示效果。
	 * 
	 * @param AbilitySystemComponent 角色的能力系统组件，用于监听属性变化
	 * @param CharacterDefinition 角色定义数据资产，包含头像图标等信息
	 * @param bIsFriendly 标识该单位是否为友方单位，影响头像边框颜色显示
	 * @param bIsSelf 标识该单位是否为当前玩家自己，自己头像会有特殊边框颜色
	 */
	void SetAscAndPortrait(UAbilitySystemComponent* AbilitySystemComponent, const UPDA_CharacterDefinition* CharacterDefinition, bool bIsFriendly, bool bIsSelf = false);

private:
	// 血量变化回调函数
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void SetHealthValue(float NewValue, float NewMaxValue);

	// 血量变化回调函数
	void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnMaxManaChanged(const FOnAttributeChangeData& Data);
	void SetManaValue(float NewValue, float NewMaxValue);

	// 死亡标签更新回调函数
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);

	void SetLevelValue(const FOnAttributeChangeData& Data);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> BarBox;
	
	// 头像边界
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> PortraitBorder;
	
	// 头像图像控件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PortraitImage;
	// 材质参数名称：图标纹理参数（用于动态材质调整）
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName IconTextureMatParamName = "Icon";
	// 材质参数名称：饱和度参数（用于选中状态高亮）
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName SaturationMatParamName = "Saturation";

	// 血量条控件
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	// 蓝量条控件
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> ManaBar;

	// 死亡倒数计时器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NumberText;

	// 角色等级
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LevelText;
	// 数字格式化选项（整数）
	FNumberFormattingOptions WholeNumberFormattingOptions;

	// 数字格式化选项（一位小数）
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;
	// 缓存生命、最大生命、法力、最大法力
	float CacheHealth;
	float CacheMaxHealth;
	float CacheMana;
	float CacheMaxMana;

	// 控件拥有者的能力系统组件
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerAbilitySystemComponent;
	
	// 关联死亡效果的句柄
	FActiveGameplayEffectHandle OwnerEffectHandle;
	// 更新定时器
	FTimerHandle UpdateTimerHandle;
	// 更新显示
	void UpdateTimer();
};