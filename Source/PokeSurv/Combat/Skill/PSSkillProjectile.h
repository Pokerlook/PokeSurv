// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Combat/Skill/PSSkillBase.h"
#include "PSSkillProjectile.generated.h"

class APSProjectileBase;

UENUM(BlueprintType)
enum class EProjectileAimMode : uint8
{
	NearestEnemy,   // 가장 가까운 적
	MoveDirection,  // 이동 방향
	Random,         // 랜덤 방향
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class POKESURV_API UPSSkillProjectile : public UPSSkillBase
{
	GENERATED_BODY()

protected:
	virtual void Activate() override;
	virtual void FireOnce() override;

private:
	FVector GetFireDirection(const FVector& StartLocation);
	FVector GetNearestEnemyDirection(const FVector& StartLocation);
	FVector GetMoveDirection();
	
	APSProjectileBase* RequestProjectile(FVector Location, FRotator Rotation);

	FVector GetSpawnOffset();
	void RefillSpawnOffsets();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EProjectileAimMode AimMode = EProjectileAimMode::NearestEnemy;

private:
	TArray<FVector> RemainingSpawnOffsets;
};
