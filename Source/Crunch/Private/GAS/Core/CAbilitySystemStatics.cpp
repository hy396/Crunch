// 幻雨喜欢小猫咪


#include "GAS/Core/CAbilitySystemStatics.h"

#include "TGameplayTags.h"

FGameplayTag UCAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	// 如果不喜欢用代码创建tag可以用下列方法，感觉有打错字的风险
	// 可以使用这种方法来获取到tag
	//return FGameplayTag::RequestGameplayTag("ability.basic attack");

	// 在cpp函数中定义的tag的获取方法如下
	return TGameplayTags::Ability_BasicAttack;
}
