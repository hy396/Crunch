// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SessionEntryWidget.generated.h"


// 定义一个多播委托，当玩家点击某个会话条目时触发
// 参数：选中的 SessionId 字符串
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString& /*SelectedSessionIdStr*/)
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString&, SelectedSessionIdStr);

/**
 * 会话列表中的一个条目（用于显示会话名称和点击按钮）
 */
UCLASS()
class CRUNCH_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:	
	virtual void NativeConstruct() override;

	// 点击时对外广播的事件
	// UPROPERTY(BlueprintAssignable)
	FOnSessionEntrySelected OnSessionEntrySelected;

	// 用于初始化显示的内容（会话名称、ID 和人数信息）
	UFUNCTION(BlueprintCallable)
	void InitializeEntry(const FString& Name, const FString& SessionIdStr, int32 CurrentPlayers = 0, int32 MaxPlayers = 0);

	// 根据房间状态更新颜色
	void UpdateRoomStatusColor();

	// 获取缓存的 SessionId
	FORCEINLINE FString GetCachedSessionIdStr() const { return CachedSessionIdStr; }

	// 获取当前人数信息
	
	FORCEINLINE int32 GetCurrentPlayers() const { return CachedCurrentPlayers; }
	FORCEINLINE int32 GetMaxPlayers() const { return CachedMaxPlayers; }
	
	// 检查房间是否已满
	FORCEINLINE bool IsRoomFull() const { return CachedCurrentPlayers >= CachedMaxPlayers; }
	
	// 检查房间是否接近满员（80%以上）
	FORCEINLINE bool IsRoomNearlyFull() const { return CachedCurrentPlayers >= CachedMaxPlayers * 0.8f; }

private:
	// 会话按钮
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SessionButton;

	// 会话名称和人数信息显示（合并）
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SessionNameText;

	// 缓存的 SessionId
	FString CachedSessionIdStr;

	// 缓存的房间人数信息（用于状态判断）
	int32 CachedCurrentPlayers;
	int32 CachedMaxPlayers;

	// 会话按钮点击时调用
	UFUNCTION()
	void SessionEntrySelected();
};
