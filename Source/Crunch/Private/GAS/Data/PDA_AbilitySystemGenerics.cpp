// 幻雨喜欢小猫咪


#include "PDA_AbilitySystemGenerics.h"

const FRealCurve* UPDA_AbilitySystemGenerics::GetExperienceCurve() const
{
	return ExperienceCurveTable->FindCurve(ExperienceRowName, "");
}
