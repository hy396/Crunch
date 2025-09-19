// 幻雨喜欢小猫咪


#include "Player/NumberPopComponent_NiagaraText.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

UNumberPopComponent_NiagaraText::UNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	// bReplicates = true;
}

void UNumberPopComponent_NiagaraText::AddNumberPop(const FNumberPopRequest& NewRequest, FGameplayTag DamageType)
{
	// int32 LocalDamage = NewRequest.NumberToDisplay;
	//
	// //Change Damage to negative to differentiate Critial vs Normal hit
	// // 如果是致命伤害，则将数值设为负数以区分普通伤害
	// if (NewRequest.bIsCriticalDamage)
	// {
	// 	LocalDamage *= -1;
	// }
	//
	// // 如果没有 Niagara 组件，则创建一个
	// if (!NiagaraComp)
	// {
	// 	NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
	// 	if (TextNiagara != nullptr)
	// 	{
	// 		NiagaraComp->SetAsset(TextNiagara);			// 设置 Niagara 资源
	// 		NiagaraComp->bAutoActivate = false;				// 不自动激活
	// 	}
	// 	NiagaraComp->SetupAttachment(nullptr);      // 不附加到任何物体
	//
	// 	// 新加的 Begin
	// 	//NiagaraComp->SetForceLocalPlayerEffect(false);	 // 允许所有客户端显示
	// 	//NiagaraComp->SetRenderingEnabled(true);			// 确保渲染启用
	// 	//NiagaraComp->SetAutoDestroy(false);				// ✅ 禁用自动销毁
	// 	// 新加的 End
	// 	
	// 	check(NiagaraComp);
	// 	NiagaraComp->RegisterComponent();					// 注册组件以便更新和渲染
	// 	// NiagaraComp->SetReplicate(true);
	// }
	//
	// NiagaraComp->Activate(false);                     // 手动激活 Niagara 粒子效果
	// NiagaraComp->SetWorldLocation(NewRequest.WorldLocation); // 设置弹出位置
	//
	// UE_LOG(LogTemp, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	// //Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	// // 获取 Niagara 数组中的 FVector4 列表（XYZ 表示位置，W 表示伤害值）
	// TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NiagaraComp, NiagaraArrayName);
	//
	// // 添加新伤害信息到数组中
	// DamageList.Add(FVector4(
	// 	NewRequest.WorldLocation.X,
	// 	NewRequest.WorldLocation.Y,
	// 	NewRequest.WorldLocation.Z,
	// 	LocalDamage));
	//
	// // 将更新后的数组写回 Niagara 组件UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, NiagaraArrayName, DamageList);
	// UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, NiagaraArrayName, DamageList);
	if (DamageType.MatchesTagExact(TGameplayTags::DamageType_AttackDamage))
	{
		AddAttackNumber(NewRequest);
	}else if (DamageType.MatchesTagExact(TGameplayTags::DamageType_MagicDamage))
	{
		AddMagicNumber(NewRequest);
	}else if (DamageType.MatchesTagExact(TGameplayTags::DamageType_TrueDamage))
	{
		AddTrueNumber(NewRequest);
	}
}

void UNumberPopComponent_NiagaraText::AddAttackNumber(const FNumberPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;

	//Change Damage to negative to differentiate Critial vs Normal hit
	// 如果是致命伤害，则将数值设为负数以区分普通伤害
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	// 如果没有 Niagara 组件，则创建一个
	if (!AttackNiagaraComp)
	{
		AttackNiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (AttackTextNiagara != nullptr)
		{
			AttackNiagaraComp->SetAsset(AttackTextNiagara);			// 设置 Niagara 资源
			AttackNiagaraComp->bAutoActivate = false;				// 不自动激活
		}
		AttackNiagaraComp->SetupAttachment(nullptr);      // 不附加到任何物体
		check(AttackNiagaraComp);
		AttackNiagaraComp->RegisterComponent();					// 注册组件以便更新和渲染
		// NiagaraComp->SetReplicate(true);
	}

	AttackNiagaraComp->Activate(false);                     // 手动激活 Niagara 粒子效果
	AttackNiagaraComp->SetWorldLocation(NewRequest.WorldLocation); // 设置弹出位置
	
	UE_LOG(LogTemp, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	//Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	// 获取 Niagara 数组中的 FVector4 列表（XYZ 表示位置，W 表示伤害值）
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(AttackNiagaraComp, NiagaraArrayName);

	// 添加新伤害信息到数组中
	DamageList.Add(FVector4(
		NewRequest.WorldLocation.X,
		NewRequest.WorldLocation.Y,
		NewRequest.WorldLocation.Z,
		LocalDamage));

	// TODO: 测试的东西25/08/03，如此确实可以弹出多种伤害
	// 修改Begin
	// for (int32 i = 0; i < 10; i++)
	// {
	// 	DamageList.Add(FVector4(
	// 		NewRequest.WorldLocation.X ,
	// 		NewRequest.WorldLocation.Y,
	// 		NewRequest.WorldLocation.Z + i * 20.f,
	// 		10*i));
	// }
	// 修改End
	// 将更新后的数组写回 Niagara 组件UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, NiagaraArrayName, DamageList);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(AttackNiagaraComp, NiagaraArrayName, DamageList);
}

void UNumberPopComponent_NiagaraText::AddMagicNumber(const FNumberPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;

	//Change Damage to negative to differentiate Critial vs Normal hit
	// 如果是致命伤害，则将数值设为负数以区分普通伤害
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	// 如果没有 Niagara 组件，则创建一个
	if (!MagicNiagaraComp)
	{
		MagicNiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (MagicTextNiagara != nullptr)
		{
			MagicNiagaraComp->SetAsset(MagicTextNiagara);			// 设置 Niagara 资源
			MagicNiagaraComp->bAutoActivate = false;				// 不自动激活
		}
		MagicNiagaraComp->SetupAttachment(nullptr);      // 不附加到任何物体
		check(MagicNiagaraComp);
		MagicNiagaraComp->RegisterComponent();					// 注册组件以便更新和渲染
		// NiagaraComp->SetReplicate(true);
	}

	MagicNiagaraComp->Activate(false);                     // 手动激活 Niagara 粒子效果
	MagicNiagaraComp->SetWorldLocation(NewRequest.WorldLocation); // 设置弹出位置
	
	UE_LOG(LogTemp, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	//Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	// 获取 Niagara 数组中的 FVector4 列表（XYZ 表示位置，W 表示伤害值）
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(MagicNiagaraComp, NiagaraArrayName);

	// 添加新伤害信息到数组中
	DamageList.Add(FVector4(
		NewRequest.WorldLocation.X,
		NewRequest.WorldLocation.Y,
		NewRequest.WorldLocation.Z,
		LocalDamage));

	// 将更新后的数组写回 Niagara 组件UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, NiagaraArrayName, DamageList);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(MagicNiagaraComp, NiagaraArrayName, DamageList);
}

void UNumberPopComponent_NiagaraText::AddTrueNumber(const FNumberPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;
	// 如果是致命伤害，则将数值设为负数以区分普通伤害
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	// 如果没有 Niagara 组件，则创建一个
	if (!TrueNiagaraComp)
	{
		TrueNiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (TrueTextNiagara != nullptr)
		{
			TrueNiagaraComp->SetAsset(TrueTextNiagara);			// 设置 Niagara 资源
			TrueNiagaraComp->bAutoActivate = false;				// 不自动激活
		}
		TrueNiagaraComp->SetupAttachment(nullptr);      // 不附加到任何物体
		
		check(TrueNiagaraComp);
		TrueNiagaraComp->RegisterComponent();					// 注册组件以便更新和渲染
	}

	TrueNiagaraComp->Activate(false);                     // 手动激活 Niagara 粒子效果
	TrueNiagaraComp->SetWorldLocation(NewRequest.WorldLocation); // 设置弹出位置
	
	UE_LOG(LogTemp, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	//Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	// 获取 Niagara 数组中的 FVector4 列表（XYZ 表示位置，W 表示伤害值）
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(TrueNiagaraComp, NiagaraArrayName);

	// 添加新伤害信息到数组中
	DamageList.Add(FVector4(
		NewRequest.WorldLocation.X,
		NewRequest.WorldLocation.Y,
		NewRequest.WorldLocation.Z,
		LocalDamage));

	// 将更新后的数组写回 Niagara 组件UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, NiagaraArrayName, DamageList);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(TrueNiagaraComp, NiagaraArrayName, DamageList);
}
