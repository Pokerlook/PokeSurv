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

private:
	FVector GetFireDirection();
	FVector GetNearestEnemyDirection();
	FVector GetMoveDirection();
	APSProjectileBase* RequestProjectile(FVector Location, FRotator Rotation); // 지금은 SpawnActor, 나중에 풀에서 꺼내오는 걸로 교체


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EProjectileAimMode AimMode = EProjectileAimMode::NearestEnemy;
	
};
