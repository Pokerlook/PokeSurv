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
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void OnSpawnFromPool_Implementation() override;	
	virtual void OnReturnToPool_Implementation() override;
	
	void Initialize(ESwarmType , const FVector& InStraightDir = FVector::ZeroVector);

protected:
    virtual FVector GetMovementDirection() const override;

	void PushBlockingEnemies();
	
	static FVector CalcPushDir(const FVector& MyLoc, const FVector& MoveDir, const FVector& OtherLoc);

	
	// 밀치기 파라미터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Push")
	float PushInterval = 0.15f;   // 밀치기 체크 주기 (초)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Push")
	float PushForce = 300.f;      // 밀어내는 세기

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swarm|Push")
	float PushRadiusScale = 1.2f; // 캡슐 반지름 대비 탐색 반경
	
private:
	FTimerHandle LifeTimer;
	
	ESwarmType SwarmType;
	FVector StraightDir;
	
	float PushAccum = 0.f; 
	
};
