// 幻雨喜欢小猫咪

#include "UI/Gameplay/StatusEffect/StatusEffectWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "UI/Gameplay/StatusEffect/StatusEffectItemWidget.h"
#include "GameplayEffectTypes.h"
#include "GAS/Core/TGameplayTags.h"

void UStatusEffectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 配置WrapBox的布局属性
	if (StatusEffectContainer)
	{
		// 设置从左到右、从上到下的布局方向
		StatusEffectContainer->SetOrientation(EOrientation::Orient_Horizontal);
		// StatusEffectContainer->ClearChildren();
		
		// 注意：移除了不存在的SetWrapSizeDirection调用
		// WrapBox会自动处理换行，不需要额外设置换行方向
	}
	// 初始化状态效果数据
	InitStatusEffectData();
}

void UStatusEffectWidget::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	OwnerAbilitySystemComponent = AbilitySystemComponent;

	// 绑定状态效果添加和移除事件
	// 使用RegisterGameplayTagEvent来监听游戏效果的添加和移除
	// 这是UE 5.3中推荐的方式
	// OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Debuff, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	// OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Buff, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);

	// OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);

	// OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Debuff).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	// OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Buff).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	// OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Stun).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	// ClearAllStatusEffectItems();

	StatusEffectContainer->ClearChildren();
}

void UStatusEffectWidget::InitStatusEffectData()
{
	StatusEffectDataMap.Empty();

	if (!StatusEffectDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("状态效果数据表为空，无法初始化状态效果数据"));
		return;
	}

	static const FString Context(TEXT("InitStatusEffectData"));

	TArray<FStatusEffectData*> Rows;
	StatusEffectDataTable->GetAllRows(Context, Rows);

	StatusEffectDataMap.Reserve(Rows.Num());

	for (const FStatusEffectData* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}

		if (!Row->StatusEffectTag.IsValid())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("状态效果数据行存在无效的 StatusEffectTag"));
			continue;
		}

		if (StatusEffectDataMap.Contains(Row->StatusEffectTag))
		{
			UE_LOG(LogTemp, Warning,
				TEXT("状态效果数据表中存在重复标签：%s"),
				*Row->StatusEffectTag.ToString());
			continue;
		}

		StatusEffectDataMap.Add(Row->StatusEffectTag, *Row);
	}

	UE_LOG(LogTemp, Log,
		TEXT("状态效果数据初始化完成，数量：%d"),
		StatusEffectDataMap.Num());
}



void UStatusEffectWidget::UpdateStatusEffectItem()
{
	if (!OwnerAbilitySystemComponent)
	{
		return;
	}

	/* ---------- 1. 查询当前所有“状态类”GE ---------- */

	FGameplayTagContainer Tags;
	Tags.AddTag(TGameplayTags::Stats);
	Tags.AddTag(TGameplayTags::Status::Status);

	FGameplayEffectQuery Query;
	Query.OwningTagQuery.MakeQuery_MatchAnyTags(Tags);

	const TArray<FActiveGameplayEffectHandle> ActiveHandles =
		OwnerAbilitySystemComponent->GetActiveEffects(Query);

	/* ---------- 2. 处理新增的 ---------- */

	for (const FActiveGameplayEffectHandle& Handle : ActiveHandles)
	{
		// 已存在 → 跳过
		if (EffectHandleToWidget.Contains(Handle))
		{
			continue;
		}

		const UGameplayEffect* GE =
			UAbilitySystemBlueprintLibrary::GetGameplayEffectFromActiveEffectHandle(Handle);

		if (!GE)
		{
			continue;
		}

		// 找到匹配的 UI 数据
		for (const auto& Pair : StatusEffectDataMap)
		{
			if (!GE->GetGrantedTags().HasTag(Pair.Key))
			{
				continue;
			}

			if (UStatusEffectItemWidget* NewItem =
				CreateWidget<UStatusEffectItemWidget>(this, StatusEffectItemWidgetClass))
			{
				NewItem->Init(
					OwnerAbilitySystemComponent,
					Handle,
					Pair.Value,
					this);

				StatusEffectContainer->AddChildToWrapBox(NewItem);
				EffectHandleToWidget.Add(Handle, NewItem);
			}

			break; // 一个 GE 只对应一个 UI 数据
		}
	}

	/* ---------- 3. 移除已经失效的 ---------- */

	TArray<FActiveGameplayEffectHandle> ToRemove;

	for (auto& Pair : EffectHandleToWidget)
	{
		if (!OwnerAbilitySystemComponent->GetActiveGameplayEffect(Pair.Key))
		{
			if (Pair.Value)
			{
				Pair.Value->RemoveFromParent();
			}
			ToRemove.Add(Pair.Key);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : ToRemove)
	{
		EffectHandleToWidget.Remove(Handle);
	}
	
	// if (!OwnerAbilitySystemComponent)
	// {
	// 	return;
	// }
	// // 获取所有状态效果标签
	// FGameplayTagContainer Tags;
	// // 添加两个要查询的状态
	// Tags.AddTag(TGameplayTags::Stats);
	// Tags.AddTag(TGameplayTags::Status::Status);
	// // 创建查询，为任意标签匹配，把所有带有状态的效果的Gameplay Effect都获取到
	// const FGameplayEffectQuery Query;
	// Query.OwningTagQuery.MakeQuery_MatchAnyTags(Tags);
	// // 获取到所有匹配的Gameplay Effect
	// TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = OwnerAbilitySystemComponent->GetActiveEffects(Query);
	//
	// for (const FActiveGameplayEffectHandle& EffectHandle : ActiveEffectHandles)
	// {
	// 	// 获取游戏效果的CDO
	// 	const UGameplayEffect* GameplayEffectCDO = UAbilitySystemBlueprintLibrary::GetGameplayEffectFromActiveEffectHandle(
	// 		EffectHandle);
	// 	for (auto Pair : StatusEffectDataMap)
	// 	{
	// 		// HasMatchingGameplayTag在 5.3 中已弃用，使用GetGrantedTags().HasTag(const FGameplayTag& TagToCheck)替代
	// 		// if (GameplayEffectCDO->HasMatchingGameplayTag(Pair.Key))
	// 		// 匹配标签
	// 		if (GameplayEffectCDO->GetGrantedTags().HasTag(Pair.Key))
	// 		{
	// 			// 获取状态效果UI项
	// 			if (UStatusEffectItemWidget* StatusEffectItem = StatusEffectTagMap[Pair.Key])
	// 			{
	// 				// 更新显示
	// 				StatusEffectItem->InitStatus(EffectHandle, Pair.Value, this);
	// 			}else
	// 			{
	// 				// 创建新的
	// 				// 创建状态效果项目控件
	// 				if (UStatusEffectItemWidget* NewStatusEffectItem = CreateWidget<UStatusEffectItemWidget>(this, StatusEffectItemWidgetClass))
	// 				{
	// 					// 更新显示
	// 					NewStatusEffectItem->InitStatus(EffectHandle, Pair.Value, this);
	// 					UE_LOG(LogTemp, Warning, TEXT("EffectTags: %s, 剩余时间: %f"), *Pair.Key.ToString(), UAbilitySystemBlueprintLibrary::GetActiveGameplayEffectTotalDuration(EffectHandle));
	// 					// 添加到容器中
	// 					StatusEffectContainer->AddChildToWrapBox(NewStatusEffectItem);
	// 					// 添加到映射中
	// 					StatusEffectTagMap.Add(Pair.Key, NewStatusEffectItem);
	// 					// // 添加到激活列表中
	// 					// ActiveStatusEffectItems.Add(StatusEffectItem);
	// 					// ActiveStatusEffectHandles.Add(EffectHandle);
	// 				}
	// 			}
	// 		}
	// 	}
	// 	// 匹配标签
	// 	// GameplayEffectCDO->HasMatchingGameplayTag(TGameplayTags::Stats);
	// }
}

void UStatusEffectWidget::RemoveExpiredStatusEffectItems()
{
	if (!OwnerAbilitySystemComponent)
	{
		return;
	}

	TArray<FActiveGameplayEffectHandle> ToRemove;

	for (auto& Pair : EffectHandleToWidget)
	{
		if (!OwnerAbilitySystemComponent->GetActiveGameplayEffect(Pair.Key))
		{
			if (Pair.Value)
			{
				Pair.Value->RemoveFromParent();
			}
			ToRemove.Add(Pair.Key);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : ToRemove)
	{
		EffectHandleToWidget.Remove(Handle);
	}
}

