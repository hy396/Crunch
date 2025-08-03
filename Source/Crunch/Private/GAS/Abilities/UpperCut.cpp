// 幻雨喜欢小猫咪


#include "GAS/Abilities/UpperCut.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Core/TGameplayTags.h"

UUpperCut::UUpperCut()
{
	// 阻止带有Ability_BasicAttack标签的技能激活
	BlockAbilitiesWithTag.AddTag(TGameplayTags::Ability_BasicAttack);
}

void UUpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	// 服务器执行
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayUpperCutMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayUpperCutMontageTask->OnBlendOut.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCancelled.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCompleted.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnInterrupted.AddDynamic(this, &UUpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLaunchEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Uppercut_Launch);
		WaitLaunchEventTask->EventReceived.AddDynamic(this, &UUpperCut::StartLaunching);
		WaitLaunchEventTask->ReadyForActivation();

	}

	NextComboName = NAME_None;
}

const FGenericDamageEffectDef* UUpperCut::GetDamageEffectDefForCurrentCombo() const
{
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance)
	{
		// 获取当前片段名称
		FName CurrentComboName = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
		// 获取当前片段对应的伤害效果
		const FGenericDamageEffectDef* EffectDef = ComboDamageMap.Find(CurrentComboName);
		if (EffectDef)
		{
			return EffectDef;
		}
	}
	// 没找到返回一个空结构
	return nullptr;
}

void UUpperCut::StartLaunching(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		// 推动自己向上
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperCutLaunchSpeed);
		// 获取命中目标的数量
		int HitResultCount = UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(EventData.TargetData);

		// 对所有命中的目标执行击飞和伤害
		for (int i = 0; i < HitResultCount; i++)
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(EventData.TargetData, i);
			PushTarget(HitResult.GetActor(), FVector::UpVector * UpperCutLaunchSpeed);
			ApplyGameplayEffectToHitResultActor(HitResult, LaunchDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}

	// 监听连招切换、提交、伤害等事件
	UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Combo_Change, nullptr, false, false);
	WaitComboChangeEvent->EventReceived.AddDynamic(this, &UUpperCut::HandleComboChangeEvent);
	WaitComboChangeEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_BasicAttack_Pressed);
	WaitComboCommitEvent->EventReceived.AddDynamic(this, &UUpperCut::HandleComboCommitEvent);
	WaitComboCommitEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Combo_Damage);
	WaitComboDamageEvent->EventReceived.AddDynamic(this, &UUpperCut::HandleComboDamageEvent);
	WaitComboDamageEvent->ReadyForActivation();
}

void UUpperCut::HandleComboChangeEvent(FGameplayEventData EventData)
{
	// 获取事件标签
	FGameplayTag EventTag = EventData.EventTag;
	if (EventTag == TGameplayTags::Ability_Combo_Change_End)
	{
		// 下一个连招名称置空
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("清除连招"))
		return;
	}

	// 获取下一个连段的名称
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	
	// Tag最后一段的名称比如Combo02,03,04等
	NextComboName = TagNames.Last();
	UE_LOG(LogTemp, Warning, TEXT("下一个Combo：%s"), *NextComboName.ToString())
}

void UUpperCut::HandleComboCommitEvent(FGameplayEventData EventData)
{
	// 按晚了
	if (NextComboName == NAME_None) return;

	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (!OwnerAnimInstance) return;

	OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage), NextComboName, UpperCutMontage);
	UE_LOG(LogTemp, Warning, TEXT("触发连招：%s"), *NextComboName.ToString())
}

void UUpperCut::HandleComboDamageEvent(FGameplayEventData EventData)
{
	if (K2_HasAuthority())
	{
		// 击飞一下自己，免得掉下去了
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperComboHoldSpeed);

		// 获取当前片段对应的伤害效果
		const FGenericDamageEffectDef* EffectDef = GetDamageEffectDefForCurrentCombo();

		if (!EffectDef)
		{
			return;
		}

		int HitResultCount = UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(EventData.TargetData);
		for (int32 i = 0; i < HitResultCount; i++)
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(EventData.TargetData, i);

			// 计算推力方向（根据自身朝向变换推力向量）
			FVector PushVel = GetAvatarActorFromActorInfo()->GetActorTransform().TransformVector(EffectDef->PushVelocity);

			// 推动目标
			PushTarget(HitResult.GetActor(), PushVel);

			// 对目标应用伤害效果
			ApplyGameplayEffectToHitResultActor(HitResult, EffectDef->DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}
}
