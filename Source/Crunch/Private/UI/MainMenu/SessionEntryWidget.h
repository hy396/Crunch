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
	FOnSessionEntrySelected OnSessionEntrySelected;

	// 用于初始化显示的内容（会话名称和 ID）
	void InitializeEntry(const FString& Name, const FString& SessionIdStr);

	// 获取缓存的 SessionId
	FORCEINLINE FString GetCachedSessionIdStr() const { return CachedSessionIdStr; }

private:
	// 会话按钮
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SessionButton;

	// 会话名称
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SessionNameText;

	// 缓存的 SessionId
	FString CachedSessionIdStr;

	// 会话按钮点击时调用
	UFUNCTION()
	void SessionEntrySelected();
};
