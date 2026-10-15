// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PokeSurv/PSTypes.h"
#include "PSWaveSubsystem.generated.h"

class APSCharacterEnemy;
class APSCharacterEnemySwarm;

USTRUCT()
struct FMapEventStruct
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;

	int32 Repeats = 0;
    int32 MaxRepeats = 0;
	
	FPSMapEvent EventData; 
};

/**
 * 
 */
UCLASS()
class POKESURV_API UPSWaveSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Deinitialize() override;
	
	void StartWave(const UDataTable* InWaveTable, const TSubclassOf<APSCharacterEnemy>& InEnemyClass, const TSubclassOf<APSCharacterEnemySwarm>& InSwarmClass);

protected:
	void RowCheck();  // 행 바뀔 때 1회 처리 (하위 타이머 재설정)
	void ApplyRow();
	void SpawnRoutine();
	void MapEventTick(int32 EventID);

	FVector GetEnemySpawnLocation(float Radius);
	bool SpawnEnemy();
	bool SpawnSwarm(FPSMapEvent FMapEvent);
	void SpawnOneSwarm(const FVector& Loc, ESwarmType SwarmType,
					   const FVector& GroupDir, UPSPokemonData* Data);
	void SpawnBossesAndTreasure();
	
private:
	TSubclassOf<APSCharacterEnemy> EnemyClass;
	TSubclassOf<APSCharacterEnemySwarm> SwarmClass;
	
	TArray<FPSWaveRow> Rows;
    int32 CurrentRowIndex = 0; 
	int32 NextEnemyIndex = 0;		// FPSWaveRow, TArray<TObjectPtr<UPSPokemonData>> Enemies;
	
	FTimerHandle RowCheckTimer;     
	FTimerHandle SpawnTimer;		
	TMap<int32, FMapEventStruct> MapEventMap;		
	int32 NextMapEventID = 0;

	float EnemySpawnRadius = 625;	// BP_PlayerBase의 카메라->Ortho Width... 16:9, 가로세로로 대각길이 구하고, 그 이상
	float SwarmStraightRadius = 900;	
};
