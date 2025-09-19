// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "GAS/Core/CGameplayAbilityTypes.h"
#include "NumberPopComponent_NiagaraText.generated.h"


// 定义一个结构体，表示一个数字弹出请求的数据
USTRUCT(BlueprintType)
struct FNumberPopRequest
{
	GENERATED_BODY()

	// 弹出数字的位置（世界坐标）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lyra|Number Pops")
	FVector WorldLocation;

	// 要显示的数字
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lyra|Number Pops")
	int32 NumberToDisplay = 0;

	// 是否是“致命”伤害（@TODO: 应该使用标签来代替）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lyra|Number Pops")
	bool bIsCriticalDamage = false;

	// 构造函数，初始化默认值
	FNumberPopRequest()
		: WorldLocation(ForceInitToZero)
	{
	}
};

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class UNumberPopComponent_NiagaraText : public UControllerComponent
{
	GENERATED_BODY()
public:

	UNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** 
	 * 添加一个数字弹出到列表中以进行可视化展示
	 * @param NewRequest 新的数字弹出请求数据
	 * @param DamageType 伤害类型
	 */
	UFUNCTION(BlueprintCallable, Category = Foo)
	void AddNumberPop(const FNumberPopRequest& NewRequest, FGameplayTag DamageType);

	void AddAttackNumber(const FNumberPopRequest& NewRequest);
	void AddMagicNumber(const FNumberPopRequest& NewRequest);
	void AddTrueNumber(const FNumberPopRequest& NewRequest);
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FName NiagaraArrayName;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> AttackTextNiagara;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> MagicTextNiagara;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> TrueTextNiagara;
protected:
	
	TArray<int32> DamageNumberArray;
	
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> AttackNiagaraComp;
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> MagicNiagaraComp;
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> TrueNiagaraComp;
};
