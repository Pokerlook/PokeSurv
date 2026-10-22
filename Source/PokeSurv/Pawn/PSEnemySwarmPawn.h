// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Pawn/PSEnemyPawn.h"
#include "PSEnemySwarmPawn.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSEnemySwarmPawn : public APSEnemyPawn
{
	GENERATED_BODY()

public:
	virtual void OnReturnToPool_Implementation() override;

	// Straight면 FlowDirection 주입, Encircle이면 추적
	void Initialize(ESwarmType InSwarmType, const FVector& InFlowDir = FVector::ZeroVector);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm")
	float StraightLifeSeconds = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm")
	float EncircleLifeSeconds = 30.f;

private:
	FTimerHandle LifeTimer;
	
};
