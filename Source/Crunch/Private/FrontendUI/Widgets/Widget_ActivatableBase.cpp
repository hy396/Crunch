// 幻雨喜欢小猫咪


#include "Widget_ActivatableBase.h"

APlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	// TODO:唉这就乱写了，这都不知道是在搞鸡毛卧槽，非要指定他的玩家控制器，他的玩家控制器鸡毛没有
	// 只有当这个缓存的若对象指针无效时，我们才需要进行类型转换
	if (!CachedOwningFrontendPC.IsValid())
	{
		CachedOwningFrontendPC = GetOwningPlayer<APlayerController>();
	}
	return CachedOwningFrontendPC.IsValid()? CachedOwningFrontendPC.Get() : nullptr;
}
