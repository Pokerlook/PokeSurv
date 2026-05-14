// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PokeSurv/Interface/PSPoolable.h"
#include "PSPoolableActor.generated.h"

UCLASS()
class POKESURV_API APSPoolableActor : public AActor, public IPSPoolable
{
	GENERATED_BODY()

public:
	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;

	virtual bool ShouldCull_Implementation() override;
	virtual void CheckCulling_Implementation(const FBox2D& ScreenBounds) override;
};
