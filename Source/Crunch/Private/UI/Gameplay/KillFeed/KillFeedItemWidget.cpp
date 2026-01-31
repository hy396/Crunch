// 幻雨喜欢小猫咪

#include "KillFeedItemWidget.h"
#include "Player/MPlayerState.h"
#include "Character/PDA_CharacterDefinition.h"
#include "Components/SizeBox.h"
// #include "Animation/WidgetAnimationObject.h"

void UKillFeedItemWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UKillFeedItemWidget::SetKillFeedData(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates)
{
    if (!KillerState || !VictimState)
    {
        return;
    }

    // 清空助攻者容器
    if (AssistContainer)
    {
        AssistContainer->ClearChildren();

        // 添加助攻者头像（从左到右）
        for (AMPlayerState* AssistState : AssistStates)
        {
            if (AssistState)
            {
                // 使用与PlayerPortraitWidget相同的方式获取角色定义
                const UPDA_CharacterDefinition* AssistDef = AssistState->GetPlayerSelection().GetCharacterDefinition();
                if (AssistDef && AssistDef->LoadIcon())
                {
                    // 创建头像SizeBox
                    USizeBox* PortraitBox = NewObject<USizeBox>(this);
                    PortraitBox->SetWidthOverride(PortraitSize.X);
                    PortraitBox->SetHeightOverride(PortraitSize.Y);

                    // 创建头像Image
                    UImage* AssistPortrait = NewObject<UImage>(this);
                    AssistPortrait->SetBrushFromTexture(AssistDef->LoadIcon());

                    PortraitBox->AddChild(AssistPortrait);
                    AssistContainer->AddChild(PortraitBox);
                }
            }
        }
    }

    // 设置击杀者头像
    if (KillerPortrait)
    {
        // 使用与PlayerPortraitWidget相同的方式获取角色定义
        const UPDA_CharacterDefinition* KillerDef = KillerState->GetPlayerSelection().GetCharacterDefinition();
        if (KillerDef && KillerDef->LoadIcon())
        {
            KillerPortrait->SetBrushFromTexture(KillerDef->LoadIcon());
        }
    }

    // 设置被击杀者头像
    if (VictimPortrait)
    {
        // 使用与PlayerPortraitWidget相同的方式获取角色定义
        const UPDA_CharacterDefinition* VictimDef = VictimState->GetPlayerSelection().GetCharacterDefinition();
        if (VictimDef && VictimDef->LoadIcon())
        {
            VictimPortrait->SetBrushFromTexture(VictimDef->LoadIcon());
        }
    }

    // 设置显示时间
    SetDisplayTime(DefaultDisplayTime);
}

void UKillFeedItemWidget::SetDisplayTime(float DisplayTime)
{
    // 使用定时器在指定时间后移除这个Widget
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            FadeOutTimerHandle,
            [this, World]()
            {
                if (this)
                {
                    // 播放淡出动画
                    PlayAnimationForward(FadeOut);

                    // 动画结束后移除
                    FTimerHandle RemoveTimerHandle;
                    World->GetTimerManager().SetTimer(
                        RemoveTimerHandle,
                        [this]()
                        {
                            if (this && GetParent())
                            {
                                RemoveFromParent();
                            }
                        },
                        FadeOutTime, false);
                }
            },
            DisplayTime, false);
    }
}