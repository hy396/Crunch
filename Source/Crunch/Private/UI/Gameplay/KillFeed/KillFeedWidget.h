// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "KillFeedWidget.generated.h"

class AMPlayerState;
class UKillFeedItemWidget;

// 击杀通报数据
// USTRUCT(BlueprintType)
// struct FKillFeedData
// {
// 	GENERATED_BODY()
//
// 	// 击杀者PlayerState
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	TWeakObjectPtr<AMPlayerState> KillerPlayerState;
//
// 	// 被击杀者PlayerState
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	TWeakObjectPtr<AMPlayerState> VictimPlayerState;
//
// 	// 助攻者PlayerState列表
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	TArray<TWeakObjectPtr<AMPlayerState>> AssistPlayerStates;
//
// 	// 击杀时间
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float KillTime;
// };
/**
 * 击杀通报容器控件
 * 管理所有击杀通报条目的显示
 */
UCLASS()
class CRUNCH_API UKillFeedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 控件初始化
	virtual void NativeConstruct() override;

	/**
	 * 添加击杀通报
	 * @param KillerState 击杀者PlayerState
	 * @param VictimState 被击杀者PlayerState
	 * @param AssistStates 助攻者PlayerState列表
	 */
	UFUNCTION(BlueprintCallable, Category = "Kill Feed")
	void AddKillFeed(AMPlayerState* KillerState, AMPlayerState* VictimState, const TArray<AMPlayerState*>& AssistStates);

	/**
	 * 清空所有击杀通报
	 */
	UFUNCTION(BlueprintCallable, Category = "Kill Feed")
	void ClearAllFeed();

private:
	// 通报条目容器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> FeedContainer;

	// 最大显示条目数
	UPROPERTY(EditDefaultsOnly, Category = "Kill Feed")
	int32 MaxFeedItems = 5;

	// 击杀条目Widget类
	UPROPERTY(EditDefaultsOnly, Category = "Kill Feed")
	TSubclassOf<UKillFeedItemWidget> KillFeedItemWidgetClass;
};