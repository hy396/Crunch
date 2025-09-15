// 幻雨喜欢小猫咪

#include "UI/Gameplay/StatusEffect/StatusEffectWidget.h"
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
		
		// 注意：移除了不存在的SetWrapSizeDirection调用
		// WrapBox会自动处理换行，不需要额外设置换行方向
	}
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

	OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Debuff).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Buff).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	OwnerAbilitySystemComponent->RegisterGameplayTagEvent(TGameplayTags::Stats_Stun).AddUObject(this, &UStatusEffectWidget::OnStatusEffectTagChanged);
	ClearAllStatusEffectItems();
}

// void UStatusEffectWidget::OnStatusEffectApplied(FActiveGameplayEffectHandle EffectHandle)
// {
// 	// 这个方法目前作为备用方案，但主要依赖OnStatusEffectTagChanged
// }
//
// void UStatusEffectWidget::OnStatusEffectRemoved(FActiveGameplayEffectHandle EffectHandle)
// {
// 	// 这个方法目前作为备用方案，但主要依赖OnStatusEffectTagChanged
// }

void UStatusEffectWidget::OnStatusEffectTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// 当标签计数发生变化时，我们需要更新UI
	// 由于我们无法直接获取到具体是哪个GameplayEffect发生了变化，我们需要遍历所有激活的效果
	// 并根据数据表中的信息来决定是否显示
	
	// 清除当前所有UI项
	ClearAllStatusEffectItems();
	
	// 重新创建所有激活的状态效果UI项
	CreateAllActiveStatusEffectItems();
}

void UStatusEffectWidget::ClearAllStatusEffectItems()
{
	// 移除所有UI控件
	for (UStatusEffectItemWidget* Item : ActiveStatusEffectItems)
	{
		if (Item)
		{
			Item->RemoveFromParent();
		}
	}
	StatusEffectContainer->ClearChildren();
	// 清空数组
	ActiveStatusEffectItems.Empty();
	ActiveStatusEffectHandles.Empty();
}

void UStatusEffectWidget::CreateAllActiveStatusEffectItems()
{
	if (!OwnerAbilitySystemComponent || !StatusEffectItemWidgetClass || !StatusEffectContainer || !StatusEffectDataTable)
	{
		return;
	}

	// 获取所有激活的游戏效果
	TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = OwnerAbilitySystemComponent->GetActiveEffects(FGameplayEffectQuery());
	
	for (const FActiveGameplayEffectHandle& EffectHandle : ActiveEffectHandles)
	{
		// 获取激活的效果
		const FActiveGameplayEffect* ActiveEffect = OwnerAbilitySystemComponent->GetActiveGameplayEffect(EffectHandle);
		if (!ActiveEffect)
		{
			continue;
		}

		// 获取效果标签
		// 在UE 5.3中，InheritableOwnedTagsContainer已被弃用，使用GetGrantedTags()替代
		const FGameplayTagContainer& EffectTags = ActiveEffect->Spec.Def.Get()->GetGrantedTags();
		
		// 使用StatusEffectItemWidget的静态方法FindStatusEffectData来查找匹配的数据
		// 这样可以复用StatusEffectItemWidget中的查找逻辑，避免代码重复
		const FStatusEffectData* MatchingData = UStatusEffectItemWidget::FindStatusEffectData(EffectTags, StatusEffectDataTable);
		
		// 如果数据表中没有匹配的标签，直接跳过，不需要显示UI
		if (!MatchingData)
		{
			continue;
		}

		// 创建状态效果项目控件
		UStatusEffectItemWidget* StatusEffectItem = CreateWidget<UStatusEffectItemWidget>(this, StatusEffectItemWidgetClass);
		if (StatusEffectItem)
		{
			// 初始化状态效果项目
			StatusEffectItem->InitializeStatusEffectItem(*ActiveEffect, this);
			
			// 添加到容器中
			StatusEffectContainer->AddChildToWrapBox(StatusEffectItem);
			
			// 添加到激活列表中
			ActiveStatusEffectItems.Add(StatusEffectItem);
			ActiveStatusEffectHandles.Add(EffectHandle);
		}
	}
}
