// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterEnemy.h"
#include "PokeSurv/PSTypes.h"
#include "PSCharacterEnemySwarm.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterEnemySwarm : public APSCharacterEnemy
{
	GENERATED_BODY()

public:
	//OnSpawnFromPool_Implementation	n초후 죽어야하면
	//OnReturnToPool_Implementation
	// ShouldCull_Implementation	플레이어와 거리가 많이 떨어지면 true
	
	void Initialize(ESwarmType InSwarmType);

protected:
	//UpdateMoveVector		if문으로? 2종류 swarm이 있는데. 벽을 만드는 swarm은 원래대로 플레이어 쫓아가고, 직진 swarm은 spawn시 설정한 방향으로만

private:
	ESwarmType SwarmType;
};
