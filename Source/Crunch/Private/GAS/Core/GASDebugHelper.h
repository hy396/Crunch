// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GASDebugHelper.generated.h"

/**
 * GAS调试助手 —— 可视化GAS内部状态、监控预测失败
 *
 * 使用方式：
 *   // 在Character::BeginPlay中注册监控
 *   UGASDebugHelper::RegisterPredictionFailureMonitor(AbilitySystemComponent);
 *
 *   // 随时打印状态快照
 *   UGASDebugHelper::PrintASCState(AbilitySystemComponent, true);
 *
 *   // 在GA中使用调试宏
 *   GAS_LOG("Ability %s activated", *GetName());
 *
 * 内置控制台命令速查：
 *   showdebug abilitysystem          → 显示GAS调试面板
 *   AbilitySystem.Debug.NextTarget   → 切换调试目标
 *   log LogAbilitySystem Verbose     → 详细GAS日志
 *   net.PktLag 200                   → 模拟200ms延迟
 *   net.PacketLoss 5                 → 模拟5%丢包
 */
UCLASS()
class CRUNCH_API UGASDebugHelper : public UObject
{
	GENERATED_BODY()

public:

	/** 打印指定ASC的完整状态到日志 */
	UFUNCTION(BlueprintCallable, Category="GAS|Debug")
	static void PrintASCState(UAbilitySystemComponent* ASC, bool bVerbose = false);

	/** 打印所有活跃的GameplayEffect */
	UFUNCTION(BlueprintCallable, Category="GAS|Debug")
	static void PrintActiveEffects(UAbilitySystemComponent* ASC);

	/** 打印所有已授予的能力及其状态 */
	UFUNCTION(BlueprintCallable, Category="GAS|Debug")
	static void PrintGrantedAbilities(UAbilitySystemComponent* ASC);

	/** 打印所有属性的Base和Current值 */
	UFUNCTION(BlueprintCallable, Category="GAS|Debug")
	static void PrintAttributes(UAbilitySystemComponent* ASC);

	/** 打印网络同步状态 */
	UFUNCTION(BlueprintCallable, Category="GAS|Debug")
	static void PrintNetworkState(UAbilitySystemComponent* ASC);

	/**
	 * 注册预测失败监控
	 * 当能力激活失败或预测被拒绝时自动打印日志
	 * 在Character::BeginPlay中调用一次即可
	 */
	UFUNCTION(BlueprintCallable, Category="GAS|Debug")
	static void RegisterPredictionFailureMonitor(UAbilitySystemComponent* ASC);
};

// ================================================================
// 调试日志宏
// ================================================================

DECLARE_LOG_CATEGORY_EXTERN(LogCrunchGAS, Log, All);

#define GAS_LOG(Format, ...) \
	UE_LOG(LogCrunchGAS, Log, TEXT(Format), ##__VA_ARGS__)

#define GAS_WARN(Format, ...) \
	UE_LOG(LogCrunchGAS, Warning, TEXT(Format), ##__VA_ARGS__)

// 带ASC上下文的日志宏
#define GAS_LOG_ASC(ASC, Format, ...) \
	UE_LOG(LogCrunchGAS, Log, TEXT("[%s][%s] " Format), \
		ASC ? (ASC->GetOwnerActor() ? *ASC->GetOwnerActor()->GetName() : TEXT("NoOwner")) : TEXT("NoASC"), \
		ASC ? (ASC->GetOwnerActor() && ASC->GetOwnerActor()->HasAuthority() ? TEXT("Server") : TEXT("Client")) : TEXT("?"), \
		##__VA_ARGS__)
