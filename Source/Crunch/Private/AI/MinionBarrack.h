// 幻雨喜欢小猫咪

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinionBarrack.generated.h"

UCLASS()
class AMinionBarrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinionBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
