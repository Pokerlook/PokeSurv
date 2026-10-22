// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSTypes.generated.h"

class APSEnemyBossPawn;
class UPSSkillBase;
class UPSItemBase;
class APSPickupBase;
class UPSPokemonData;

// 여러 곳에서 공통적으로 쓸만한 enum / struct / etc...

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsInitialized);

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

UENUM(BlueprintType)
enum class ESwarmType : uint8
{
	Straight,     // 한 방향 직진
	Encircle,    // 플레이어 포위
};

USTRUCT(BlueprintType)
struct FPSBossSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APSEnemyBossPawn> BossClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPSPokemonData> BossData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Scale = FVector::OneVector;

	// 처치 시 드롭할 상자/보물 (픽업 클래스)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APSPickupBase> TreasureDrop;
};

USTRUCT(BlueprintType)
struct FPSMapEvent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnTimeAt = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPSPokemonData> SwarmToSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESwarmType SwarmType = ESwarmType::Straight;

	// 한 번 발동 시 스폰할 마리 수 (떼몰림이면 다수)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Amount = 1;

	// 발동 확률 (0~1)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Chance = 0.0f;

	// 추가 반복 횟수 (0 이상)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Repeats = 0;
	
	// 발동 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Delay = 1.0f;
	
	// 0 이하라면 영구 지속.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Duration = -1;
};

USTRUCT(BlueprintType)
struct FPSWaveRow : public FTableRowBase
{
	GENERATED_BODY()

	// 이 행이 적용되는 경과 시간 (분)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Minute = 0;

	// 이 구간에 일반 스폰될 적 풀 (여러 종 섞임)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UPSPokemonData>> Enemies;

	// 화면 내 유지할 최소 적 수 (밑돌면 즉시 보충 스폰)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnemyMinimum = 0;

	// 일반 스폰 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnInterval = 1.0f;

	// 보스/엘리트 + 보상 (이 구간에 등장, 없으면 빈 배열)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPSBossSpawn> BossesAndTreasure;

	// 맵 이벤트 (특수 떼몰림 등, 없으면 빈 배열)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPSMapEvent> MapEvents;
};