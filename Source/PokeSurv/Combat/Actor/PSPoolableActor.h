// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeSurv/Interface/Poolable.h"
#include "PSPoolableActor.generated.h"

UCLASS()
class POKESURV_API APSPoolableActor : public AActor, public IPoolable
{
	GENERATED_BODY()

public:
	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
};
