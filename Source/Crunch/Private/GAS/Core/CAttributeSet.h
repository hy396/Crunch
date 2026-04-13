// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CGameplayAbilityTypes.h"
#include "CAttributeSet.generated.h"

//宏的设置，编译时会默认给变量生成相应的Getter以及Setter函数，当前设置会生成四个函数，获取属性，获取值，设置值，以及初始化值。
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	UPROPERTY()
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
	
};



/**
 * 击杀事件数据包装类，用于在事件中传递击杀和助攻信息
 */
UCLASS(BlueprintType)
class UDeadEventPayload : public UObject
{
	GENERATED_BODY()

public:
	// 击杀者（最后造成伤害的英雄）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Killer;

	// 助攻者列表（所有符合条件的伤害来源，除了击杀者）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> AssistHeroes;
};



/**
 * 
 */
UCLASS()
class UCAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	// 用于声明哪些属性需要在网络中进行复制
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
	// 当Attribute的CurrentValue被改变之前调用
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// 仅在instant Gameplay Effect使Attribute的BaseValue改变时触发
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data) override;

	// 根据缓存的生命百分比和新最大生命值重新计算生命值
	void RescaleHealth();
	// 根据缓存的法力百分比和最大法力值重新计算法力值
	void RescaleMana();
	//virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MaxMana)

	// // 传值物理的基础伤害
	// UPROPERTY(ReplicatedUsing = OnRep_BaseAttackDamage)
	// FGameplayAttributeData BaseAttackDamage;
	// ATTRIBUTE_ACCESSORS(UCAttributeSet, BaseAttackDamage)
	// // 魔法的基础伤害
	// UPROPERTY(ReplicatedUsing = OnRep_BaseMagicDamage)
	// FGameplayAttributeData BaseMagicDamage;
	// ATTRIBUTE_ACCESSORS(UCAttributeSet, BaseMagicDamage)
	// // 真伤的基础伤害
	// UPROPERTY(ReplicatedUsing = OnRep_BaseTrueDamage)
	// FGameplayAttributeData BaseTrueDamage;
	// ATTRIBUTE_ACCESSORS(UCAttributeSet, BaseTrueDamage)

	// 物理伤害（Meta属性，服务器用完即清零，不需要网络同步）
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackDamage)

	// 法术伤害（Meta属性）
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MagicDamage;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MagicDamage)

	// 真实伤害（Meta属性）
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData TrueDamage;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, TrueDamage)

	/** 攻击力（物理攻击强度） */
	UPROPERTY(ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, AttackPower)
	
	/** 法术强度（魔法攻击强度） */
	UPROPERTY(ReplicatedUsing = OnRep_MagicPower)
	FGameplayAttributeData MagicPower;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MagicPower)
	
	// 护甲值
	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, Armor)

	/** 法术抗性（减少受到的魔法伤害） */
	UPROPERTY(ReplicatedUsing = OnRep_MagicResistance)
	FGameplayAttributeData MagicResistance;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MagicResistance)

	// 移动速度
	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveSpeed)

	// 移动加速度
	UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
	FGameplayAttributeData MoveAcceleration;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, MoveAcceleration)

	// 缓存的生命百分比
	UPROPERTY()
	FGameplayAttributeData CachedHealthPercent;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, CachedHealthPercent)

	// 缓存的法力百分比
	UPROPERTY()
	FGameplayAttributeData CachedManaPercent;
	ATTRIBUTE_ACCESSORS(UCAttributeSet, CachedManaPercent)
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
	UFUNCTION()
	void OnRep_MoveAcceleration(const FGameplayAttributeData& OldMoveAcceleration);
	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
	UFUNCTION()
	void OnRep_MagicPower(const FGameplayAttributeData& OldMagicPower);
	UFUNCTION()
	void OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance);
	// UFUNCTION()
	// void OnRep_BaseAttackDamage(const FGameplayAttributeData& OldBaseAttackDamage);
	//
	// UFUNCTION()
	// void OnRep_BaseMagicDamage(const FGameplayAttributeData& OldBaseMagicDamage);
	// UFUNCTION()
	// void OnRep_BaseTrueDamage(const FGameplayAttributeData& OldBaseTrueDamage);

private:
    // 更新连杀/连败状态
    static void UpdateKillAndDeathStreaks(UAbilitySystemComponent* KillerASC, UAbilitySystemComponent* DeadASC);

	// 设置效果属性
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	// 伤害处理函数
	void Damage(const FEffectProperties& Props, FGameplayTag DamageType, const float Damage);

	// TODO: 未来如果学不会奶瓜将改回Aura的WBP
	//显示伤害数字
	// static void ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsCriticalHit);
	static void ShowFloatingText(const FEffectProperties& Props, float Damage, bool IsCriticalHitE, FGameplayTag DamageType);

	// 用于激活角色死亡被动的函数
	void OnDeadAbility(const FEffectProperties& Props);

	// 助攻时间阈值，单位秒，在这个时间内对敌人造成伤害的角色会被计为助攻
	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	float AssistTimeThreshold = 30.f; 

	// TMap， Key为伤害来源Actor, Value为受到伤害的时间戳，用来统计助攻
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, float> RecentDamageSourcesMap;
	//void OnDeadAbility(const FGameplayEffectModCallbackData& Data);
};
