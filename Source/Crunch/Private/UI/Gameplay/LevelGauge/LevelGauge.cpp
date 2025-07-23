// 幻雨喜欢小猫咪


#include "LevelGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Core/CHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置为无小数格式
	NumberFormattingOptions.SetMaximumFractionalDigits(0);

	// 获取角色
	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn) return;

	// 通过角色获取ASC组件
	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerAbilitySystemComponent) return;

	OwnerASC = OwnerAbilitySystemComponent;

	// 构造的时候触发一次修改
	UpdateGauge(FOnAttributeChangeData());
	
	// 绑定属性变化委托：
	// 监听经验属性变化（当前经验值）
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute())
		.AddUObject(this, &ULevelGauge::UpdateGauge);
	
	// 监听升级所需经验属性变化下一级的经验值
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetNextLevelExperienceAttribute())
		.AddUObject(this, &ULevelGauge::UpdateGauge);
	
	// 监听上一级经验属性变化该等级的启始经验值
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetPrevLevelExperienceAttribute())
		.AddUObject(this, &ULevelGauge::UpdateGauge);
	
	// 监听等级属性变化（用于显示当前等级）
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute())
		.AddUObject(this, &ULevelGauge::UpdateGauge);
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data)
{
	// 查询属性值时的标记
	bool bFound;
	
	// 获取当前经验值
	float CurrentExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound) return;
	
	// 获取升级所需经验值
	float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	
	// 获取上一级经验值
	float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	
	// 获取当前等级
	float CurrentLevel = OwnerASC->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;

	// 更新等级
	LevelText->SetText(FText::AsNumber(CurrentLevel, &NumberFormattingOptions));

	// 计算进度百分比
	// 获取当前等级获取到的经验
	float Progress = CurrentExperience - PrevLevelExperience;
	// 获取到下一级等级需要获取到的经验
	float LevelExpAmt = NextLevelExperience - PrevLevelExperience;

	float Percent = Progress / LevelExpAmt;

	// 满级
	if (NextLevelExperience == 0.f)
	{
		Percent = 1.f;
	}
	// 更新进度条材质
	if (LevelProgressImage)
	{
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent);
	}
}
