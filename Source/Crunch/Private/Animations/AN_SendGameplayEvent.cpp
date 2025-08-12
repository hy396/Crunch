// 幻雨喜欢小猫咪


#include "Animations/AN_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"

void UAN_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner()) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;

	// TODO:我试图在本地发送GameplayTag的骨骼位置(数据发不过去，不着地为啥)
	// 默认发射位置为角色位置
	// FVector SocketLocation = MeshComp->GetOwner()->GetActorLocation();
	// //UE_LOG(LogTemp, Warning, TEXT("发射位置1：%s"), *SocketLocation.ToString())
	// // 获取角色的骨骼
	// if (EventTag.IsValid())
	// {
	// 	// 如果事件标签包含Socket名，则用Socket位置
	// 	TArray<FName> OutNames;
	// 	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, OutNames);
	// 	if (OutNames.Num() != 0)
	// 	{
	// 		FName SocketName = OutNames.Last();
	// 		//UE_LOG(LogTemp, Warning, TEXT("SocketName：%s"), *SocketName.ToString())
	// 		SocketLocation = MeshComp->GetSocketLocation(SocketName);
	// 		UE_LOG(LogTemp, Warning, TEXT("发射位置0：%s"), *SocketLocation.ToString())
	// 	}
	// }
	// // 创建数据
	// FGameplayEventData Data;
	// // 添加位置
	// Data.ContextHandle.AddOrigin(SocketLocation);
	// // 发送一个指定EventTag的空Gameplay事件给OwnerActor
	// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Data);
	//
	// 发送一个指定EventTag的空Gameplay事件给OwnerActor
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}

FString UAN_SendGameplayEvent::GetNotifyName_Implementation() const
{
	// 用来让这个动画编辑界面美观点
	
	// 检查 EventTag 是否有效
	if (EventTag.IsValid())
	{
		// 将 GameplayTag 拆分为多个 FName 层级（例如 "Ability.Combo.Change.Combo02" → ["Ability", "Combo", "Change", "Combo02"]）
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	
		// 返回最后一级名称作为通知名称（例如 "Combo02"）
		return TagNames.Last().ToString();
	}

	// 如果 EventTag 无效，返回默认名称 "None"
	return "None";
}
