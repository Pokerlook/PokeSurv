// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSTypes.generated.h"

class UPSSkillBase;
class UPSItemBase;

// 여러 곳에서 공통적으로 쓸만한 enum / struct...

USTRUCT(BlueprintType)
struct FDamageResult
{
	GENERATED_BODY()
    
	UPROPERTY(BlueprintReadWrite)
	float FinalDamage = 0.f;
	
    UPROPERTY(BlueprintReadWrite)
	bool bIsCritical = false;
};

UENUM(BlueprintType)
enum class ESummonSpawnPattern  : uint8
{
    OwnerCenter,			// 캐릭터 중앙 (마늘)
	OwnerLeftRightStack,		// 캐릭터 좌우, 위치는 점점 위로. 채찍
	RandomAllEnemy,				// 랜덤. 번개반지
};

UENUM(BlueprintType)
enum class ESummonBehavior : uint8
{
	Stationary,         // 
	AttachOwner,        // 
	ChasePlayer,        // 
};

UENUM(BlueprintType)
enum class ESummonDamageMode : uint8
{
	OneShot,		// 스폰 시 1번만 (번개반지)
	Periodic,		// 콜리전에 들어오면 1번, 그리고는 주기적으로 (마늘, 장판)
};

USTRUCT(BlueprintType)
struct FPSLevelUpOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bIsSkill = true;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UPSSkillBase> SkillClass;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UPSItemBase> ItemClass;
};
