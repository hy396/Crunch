// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
// #include "Player/NumberPopComponent_NiagaraText.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI) // 添加 NotBlueprintable 避免警告
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 声明为网络调用（客户端执行）
	// UFUNCTION(BlueprintCallable, Category = "Combat", BlueprintNativeEvent)
	// void AddNiagaraText(const FNumberPopRequest& Request);
	// ✅ 修正后的声明：仅保留 Client RPC 和 BlueprintCallable
	// UFUNCTION(Client, Reliable,BlueprintCallable, Category = "Combat")
	// virtual void AddNiagaraText(const FNumberPopRequest& Request) = 0;

	/**
	 * 执行运动扭曲
	 * @param TargetLocation 目标位置
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddOrUpdateWarpTargetFromLocation(const FVector& TargetLocation);

};
