// 幻雨喜欢小猫咪


#include "Minion.h"

#include "Kismet/GameplayStatics.h"
#include "UI/Gameplay/OverHeadStatsGauge.h"


void AMinion::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	Super::SetGenericTeamId(NewTeamId);
	PickSkinBasedOnTeamID();
}

void AMinion::PickSkinBasedOnTeamID()
{
	TObjectPtr<USkeletalMesh>* Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(*Skin);
		// UE_LOG(LogTemp, Warning, TEXT("当前角色 TeamID: %u"), GetGenericTeamId().GetId());
		// 设置头顶UI颜色
		SetOverHeadWidgetColor();
	}
	
}

void AMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}
