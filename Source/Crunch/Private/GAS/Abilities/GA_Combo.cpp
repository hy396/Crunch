// 幻雨喜欢小猫咪


#include "GAS/Abilities/GA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GAS/Core/CAbilitySystemStatics.h"
#include "GAS/Core/TGameplayTags.h"

UGA_Combo::UGA_Combo()
{
	//两种方法都能将Tag放过去
	//AbilityTags.AddTag(TGameplayTags::Ability_BasicAttack);
	// 添加“基础攻击”标签，标识该技能为基础攻击
	AbilityTags.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
	// 设置互斥标签，防止同一时间释放多个基础攻击
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// 检查能否释放技能（如消耗、冷却等），不能则直接结束
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	// 服务器或预测端：播放Montage并监听动画和连招事件
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 蒙太奇任务代理
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		// 设置动画结束时的回调：当动画自然结束时结束技能
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		// 设置动画被取消时的回调：当动画被手动取消时结束技能
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		// 设置动画完成时的回调：当动画正常播放完成时结束技能
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		// 设置动画被中断时的回调：当动画被其他事件中断时结束技能
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		// 调用此函数激活任务，实际开始播放动画
		PlayComboMontageTask->ReadyForActivation();

		// 监听“连招切换”事件（如动画通知触发）
		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TGameplayTags::Ability_Combo_Change, nullptr, false, false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UGA_Combo::ComboChangedEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	// 初始化下一个连招段名称为空
	NextComboName = NAME_None;
	// 设置监听玩家输入（用于连击）
	SetupWaitComboInputPress();
}

void UGA_Combo::SetupWaitComboInputPress()
{
	// 创建监听输入任务
	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);

	// 绑定输入响应函数
	WaitInputPressTask->OnPress.AddDynamic(this, &UGA_Combo::HandleInputPress);
	WaitInputPressTask->ReadyForActivation(); // 执行任务
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	// 设置跳跃的操作
	// JumpComboName = NextComboName;
	
	// 重新设置监听输入任务
	SetupWaitComboInputPress();
	// 尝试切换到下一个连招段
	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
{
	// 没有下一招直接返回
	if (NextComboName == NAME_None)
	{
		return;
	}
	// 2025/7/5 新添加
	// if (JumpComboName == NAME_None) return;

	UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
	if (!OwnerAnimInst) return;

	// 原：
	// 设置蒙太奇自动切换到下一个片段，达成连击的效果
	OwnerAnimInst->Montage_SetNextSection(OwnerAnimInst->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *NextComboName.ToString());

	// 2025/7/5 新添加
	// 高端操作
	// // 直接跳到片段
	//OwnerAnimInst->Montage_JumpToSection(JumpComboName, ComboMontage);
	//UE_LOG(LogTemp, Warning, TEXT("Jump To %s"), *JumpComboName.ToString());
	//NextComboName = NAME_None;
	//JumpComboName = NAME_None;
}

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;

	// 如果是结束连击标签传过来了就退出
	if (EventTag == TGameplayTags::Ability_Combo_Change_End)
	{
		NextComboName = NAME_None;
		// // 2025/7/5 新添加
		// TryCommitCombo();
		
		UE_LOG(LogTemp, Log, TEXT("Combo End"))
		return;
	}

	// 获取下一个连段的名称
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);

	// Tag最后一段的名称比如Combo02,03,04等
	NextComboName = TagNames.Last();
	UE_LOG(LogTemp, Log, TEXT("Combo Changed to %s"), *NextComboName.ToString())
}
