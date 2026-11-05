// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PokeSurv/PSTypes.h"
#include "PSWaveSubsystem.generated.h"

class APSEnemyPawn;
class APSEnemySwarmPawn;

USTRUCT()
struct FMapEventStruct
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;

	int32 Repeats = 0;
    int32 MaxRepeats = 0;
	
	FPSMapEvent EventData; 
};

struct FSpawnArea
{
	FVector2D Center = FVector2D::ZeroVector;
	float W = 0.f, H = 0.f;		// 반너비/반높이 + Margin
    float Radius = 0.f;       // 반대각선 + Margin (원형 스폰용)
	float Z = 0.f;
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
	
	void StartWave(const UDataTable* InWaveTable, const TSubclassOf<APSEnemyPawn>& InEnemyClass, const TSubclassOf<APSEnemySwarmPawn>& InSwarmClass);

private:
	void RowCheck();  // 행 바뀔 때 1회 처리 (하위 타이머 재설정)
	void ApplyRow();
	void SpawnRoutine();
	void MapEventTick(int32 EventID);

	void TryBeginFirstRow();

	bool SpawnEnemy(const FSpawnArea& Area);
	bool SpawnSwarm(FPSMapEvent FMapEvent);
	void SpawnOneSwarm(const FVector& Loc, ESwarmType SwarmType,
					   const FVector& GroupDir, UPSPokemonData* Data);
	void SpawnBossesAndTreasure();
	
	float GetFloorZ() const;	// 플레이어 발밑 바닥 Z (트레이스 실패 시 캡슐 하단으로 폴백)
	float GetSpawnZ(TSubclassOf<APSEnemyPawn> InClass) const;	// 바닥 Z + 해당 클래스 캡슐 하프하이트
	
	FSpawnArea MakeSpawnArea(float Margin, TSubclassOf<APSEnemyPawn> InClass);
	static FVector RandomOnCircle(const FSpawnArea& A);
	
private:
	TSubclassOf<APSEnemyPawn> EnemyClass;
	TSubclassOf<APSEnemySwarmPawn> SwarmClass;
	
	TArray<FPSWaveRow> Rows;
    int32 CurrentRowIndex = 0; 
	int32 NextEnemyIndex = 0;		// FPSWaveRow, TArray<TObjectPtr<UPSPokemonData>> Enemies;
	
	FTimerHandle RowCheckTimer;     
	FTimerHandle SpawnTimer;		
	TMap<int32, FMapEventStruct> MapEventMap;		
	int32 NextMapEventID = 0;

	float SpawnMargin = 70.f;            // 화면 밖 여유 (스프라이트 크기 정도)
	float EncircleScale = 0.8f;          // 포위 타원 = 화면 반크기 × 이 값 (1 미만이면 화면 안)
};
