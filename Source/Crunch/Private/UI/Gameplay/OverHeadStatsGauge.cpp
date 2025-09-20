// 幻雨喜欢小猫咪


#include "UI/Gameplay/OverHeadStatsGauge.h"

#include "GenericTeamAgentInterface.h"
#include "Character/CCharacter.h"
#include "GAS/Core/CAttributeSet.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"
#include "GAS/Core/CHeroAttributeSet.h"

void UOverHeadStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();
	// 设置为整数格式
	NumberFormattingOptions.MaximumFractionalDigits = 0;
}

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
		if(!bPlayerNameTextVisible){
			bPlayerNameSet = true;
		}
		if (bShowLevel)
		{
			// 设置等级
			bool bFound;
			float AttributeValue = AbilitySystemComponent->GetGameplayAttributeValue(UCHeroAttributeSet::GetLevelAttribute(), bFound);
			if (bFound)
			{
				FOnAttributeChangeData OnLevelChangeData;
				OnLevelChangeData.NewValue = AttributeValue;
				SetLevelValue(OnLevelChangeData);
			}
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &UOverHeadStatsGauge::SetLevelValue);
		}
	}
}

void UOverHeadStatsGauge::SetPlayerNameFromPlayerState(APlayerState* PlayerState)
{
	if (!bPlayerNameTextVisible) return;
	if (!PlayerNameText || !PlayerState) return;
	FString PlayerName = PlayerState->GetPlayerName();
	PlayerNameText->SetText(FText::FromString(PlayerName));
	bPlayerNameSet = true;
}

void UOverHeadStatsGauge::SetHealthBarColor(ETeamAttitude::Type TargetTeam)
{
	if (HealthBar)
	{
		switch (TargetTeam)
		{
			case ETeamAttitude::Friendly:
				// 队友就不用管了，本来就绿绿的（本来我不想管的，后来AI不稳定）
				{
					HealthBar->SetBarColor(FriendlyColor);
					// 换成一种浅蓝色
					PlayerNameText->SetColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 1.0f)));
					// 等级的边界颜色
					if (bShowLevel)
					{
						LevelBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 1.0f, 1.0f));
					}
				}
				//HealthBar->SetBarColor(HealthBar->FriendlyColor);
				break;
			case ETeamAttitude::Hostile:
				{
					HealthBar->SetBarColor(HostileColor);
					PlayerNameText->SetColorAndOpacity(HostileColor);
					// 等级的边界颜色
					if (bShowLevel)
					{
						LevelBorder->SetBrushColor(HostileColor);
					}
				}
				//HealthBar->SetBarColor(HealthBar->HostileColor);
				break;
			default:
				{
					HealthBar->SetBarColor(FLinearColor(1.0f, 0.8f, 0.2f));
					// 默认要什么颜色好呢 - 现在是偏橙色的黄色
					PlayerNameText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.8f, 0.2f)));
					// 等级的边界颜色
					if (bShowLevel)
					{
						LevelBorder->SetBrushColor(FLinearColor(1.0f, 0.8f, 0.2f, 1.0f));
					}
				}
				break;
		}
	}
}

void UOverHeadStatsGauge::SetLevelValue(const FOnAttributeChangeData& Data)
{
	PlayerLevelText->SetText(FText::AsNumber(Data.NewValue, &NumberFormattingOptions));
}
