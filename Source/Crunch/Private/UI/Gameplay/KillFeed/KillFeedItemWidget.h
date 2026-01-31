// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "KillFeedItemWidget.generated.h"

class AMPlayerState;
class UPlayerPortraitWidget;

/**
 * 击杀通报条目控件
 * 显示单条击杀信息，包括击杀者、被击杀者和助攻者
 */
UCLASS()
class CRUNCH_API UKillFeedItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 控件初始化
    virtual void NativeConstruct() override;

    /**
     * 设置击杀通报信息
     * @param KillerState 击杀者PlayerState
     * @param VictimState 被击杀者PlayerState
     * @param AssistStates 助攻者PlayerState列表
     */
    UFUNCTION(BlueprintCallable, Category = "Kill Feed")
    void SetKillFeedData(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates);

    /**
     * 设置显示时间（用于自动隐藏）
     * @param DisplayTime 显示持续时间（秒）
     */
    UFUNCTION(BlueprintCallable, Category = "Kill Feed")
    void SetDisplayTime(float DisplayTime);

private:
    // 主容器：水平排列所有元素
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> MainContainer;

    // 助攻者容器：左侧水平排列
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UHorizontalBox> AssistContainer;

    // 击杀者头像
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> KillerPortrait;

    // 击杀图标
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> KillActionIcon;

    // 被击杀者头像
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> VictimPortrait;

    // 头像尺寸
    UPROPERTY(EditDefaultsOnly, Category = "Kill Feed")
    FVector2D PortraitSize = FVector2D(32.f, 32.f);

    // 头像间距
    UPROPERTY(EditDefaultsOnly, Category = "Kill Feed")
    float PortraitSpacing = 5.f;

    // 显示持续时间（秒）
    UPROPERTY(EditDefaultsOnly, Category = "Kill Feed")
    float DefaultDisplayTime = 5.f;

    // 淡出动画时间
    UPROPERTY(EditDefaultsOnly, Category = "Kill Feed")
    float FadeOutTime = 1.f;

    // 淡出动画
    UPROPERTY(Transient, meta=(BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> FadeOut;

    // 淡出定时器句柄
    FTimerHandle FadeOutTimerHandle;
};