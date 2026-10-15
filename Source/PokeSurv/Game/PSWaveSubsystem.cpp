// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSWaveSubsystem.h"

#include "PSPoolSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "PokeSurv/Character/PSCharacterEnemy.h"
#include "PokeSurv/Character/PSCharacterEnemyBoss.h"
#include "PokeSurv/Character/PSCharacterEnemySwarm.h"
#include "PokeSurv/Character/PSStatComponent.h"


void UPSWaveSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(RowCheckTimer);
		TM.ClearTimer(SpawnTimer);
		for (auto& Pair : MapEventMap)
			TM.ClearTimer(Pair.Value.TimerHandle);
	}
	MapEventMap.Empty();
	Super::Deinitialize();
}

void UPSWaveSubsystem::StartWave(const UDataTable* InWaveTable, const TSubclassOf<APSCharacterEnemy>& InEnemyClass, const TSubclassOf<APSCharacterEnemySwarm>& InSwarmClass)
{
	if (!GetWorld()) return;
	
	checkf(InWaveTable, TEXT("UPSWaveSubsystem::StartWave. WaveTable null"));
	checkf(InEnemyClass, TEXT("UPSWaveSubsystem::StartWave. EnemyClass null"));
	
	EnemyClass = InEnemyClass;
	SwarmClass = InSwarmClass;

	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	if (!Pool) return;
	Pool->InitPool(InEnemyClass, 300);	// 적의 수 300 이상은 스폰 하지 않을 것. (맵 이벤트, 보스 제외)
	Pool->InitPool(InSwarmClass, 150);     
	
	TArray<FPSWaveRow*> RowPtrs;
	InWaveTable->GetAllRows<FPSWaveRow>(TEXT("StartWave"), RowPtrs);
	
	Rows.Empty();
	Rows.Reserve(RowPtrs.Num());
	for (const FPSWaveRow* Ptr : RowPtrs)
	{
		if (Ptr) Rows.Add(*Ptr);   
	}

	// 행을 FPSWaveRow.Minute 기준 오름차순 정렬 (데이터 입력 순서 보장 안 되니)
	Rows.Sort([](const FPSWaveRow& A, const FPSWaveRow& B)
	{
        return A.Minute < B.Minute;
	});
	
	checkf(Rows.Num() > 0, TEXT("UPSWaveSubsystem::StartWave. WaveTable에 행이 없음"));
	
	GetWorld()->GetTimerManager().SetTimer(
		RowCheckTimer, this, &UPSWaveSubsystem::RowCheck, 60.0f, true);

	ApplyRow();  // 시작 즉시 1회 (0분 행 바로 적용)
}

void UPSWaveSubsystem::RowCheck()
{
	if (CurrentRowIndex+1 >= Rows.Num())
	{
		if (UWorld* World = GetWorld())
			World->GetTimerManager().ClearTimer(RowCheckTimer);
		return;
	}

	CurrentRowIndex++;
	ApplyRow();
}

void UPSWaveSubsystem::ApplyRow()
{
	UWorld* World = GetWorld();
	if (!World) return;
	FTimerManager& TM = World->GetTimerManager();

	// 타이머 clear
	TM.ClearTimer(SpawnTimer);
	for (auto& Pair : MapEventMap)
		TM.ClearTimer(Pair.Value.TimerHandle);
	MapEventMap.Empty();
	
	const FPSWaveRow& Row = Rows[CurrentRowIndex];
	
	// 보스(죽이면 상자 뱉 or not)
	SpawnBossesAndTreasure();
	
	// 일반 몹 스폰 타이머
	if (Row.Enemies.Num() > 0 && Row.SpawnInterval > 0.f)
	{
		NextEnemyIndex = 0;
		TM.SetTimer(SpawnTimer, this, &UPSWaveSubsystem::SpawnRoutine, Row.SpawnInterval, true);
		SpawnRoutine();
	}

	NextMapEventID = 0;
	// 맵 이벤트 타이머
	for (const auto& E : Row.MapEvents)
	{
		const int32 ID = NextMapEventID++;
		FMapEventStruct& MapEvent = MapEventMap.Add(ID);
		MapEvent.MaxRepeats = E.Repeats;
		MapEvent.EventData  = E;  

		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(
			this, &UPSWaveSubsystem::MapEventTick, ID);
		TM.SetTimer(MapEvent.TimerHandle, Delegate, E.Delay, true, E.SpawnTimeAt);
	}

	if (CurrentRowIndex + 1 >= Rows.Num())
	{
		World->GetTimerManager().ClearTimer(RowCheckTimer);
	}
}

void UPSWaveSubsystem::SpawnRoutine()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	UPSPoolSubsystem* Pool = World->GetSubsystem<UPSPoolSubsystem>();
	if (!Pool) return;
	
	SpawnEnemy();

	// 최소 수 까지 스폰
	int32 Alive = Pool->GetActiveCount(EnemyClass);
	UE_LOG(LogTemp, Warning, TEXT("[Wave] Alive:%d / Min:%d"), Alive, Rows[CurrentRowIndex].EnemyMinimum);
	
	while( Alive < Rows[CurrentRowIndex].EnemyMinimum)
	{
		if (!SpawnEnemy()) break;
		++Alive; 
	}
}

void UPSWaveSubsystem::MapEventTick(int32 EventID)
{
	if (!GetWorld()) return;
	FMapEventStruct* EventStr = MapEventMap.Find(EventID);
	if (!EventStr) return;

	EventStr->Repeats++;

	if (FMath::FRand() < EventStr->EventData.Chance)
	{
		SpawnSwarm(EventStr->EventData);   // Amount 반복은 SpawnSwarm 안에서
	}
	
	if (EventStr->Repeats > EventStr->MaxRepeats)		// 이렇게 해야 repeat=1 일 때, 최초 1회 하고 다음 1번은 더 함. >=은 1번만함. 최초가 E.SpawnTimeAt으로 타이머 되어있어서
	{
		FTimerHandle Handle = EventStr->TimerHandle;
		MapEventMap.Remove(EventID);
		GetWorld()->GetTimerManager().ClearTimer(Handle);
	}
}

FVector UPSWaveSubsystem::GetEnemySpawnLocation(float Radius)
{
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();	// 폰&컨트롤러 없으면 터질 수도 있지만 안 터질듯
	const FVector PlayerLoc = Player->GetActorLocation();

	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	
	const FVector2D Offset(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius);

	return PlayerLoc + FVector(Offset.X, Offset.Y, 0.f);
}

bool UPSWaveSubsystem::SpawnEnemy()
{
	UWorld* World = GetWorld();
	if (!World) return false;

	const FPSWaveRow& Row = Rows[CurrentRowIndex];
	if (Row.Enemies.Num() == 0) return false;  // % 0, empty.
	
	UPSPokemonData* Data = Row.Enemies[NextEnemyIndex];
	//if (!Data) return false;		Data null 이면 터지도록
	NextEnemyIndex = (NextEnemyIndex + 1) % Row.Enemies.Num();
	
	UPSPoolSubsystem* Pool = World->GetSubsystem<UPSPoolSubsystem>();
	if (!Pool) return false;

	APSCharacterEnemy* Enemy = Cast<APSCharacterEnemy>(
		Pool->SpawnFromPool(EnemyClass, GetEnemySpawnLocation(EnemySpawnRadius), FRotator::ZeroRotator));
	if (!Enemy) return false;  // 풀 고갈 → 스킵

	UPSStatComponent* Stat = Enemy->FindComponentByClass<UPSStatComponent>();
	Stat->Initialize(Data);  // 없으면 터지지만 의도된 것
	return true;
}

bool UPSWaveSubsystem::SpawnSwarm(FPSMapEvent FMapEvent)
{
	UWorld* World = GetWorld();
	if (!World) return false;
	
	UPSPokemonData* Data = FMapEvent.SwarmToSpawn;
    checkf(Data, TEXT("SpawnSwarm: EnemyToSpawn이 null — 테이블 확인"));

	UPSPoolSubsystem* Pool = World->GetSubsystem<UPSPoolSubsystem>();
	if (!Pool) return false;
	
	if(FMapEvent.SwarmType == ESwarmType::Straight)
	{
		// 이 무리의 중심점 하나 잡고, 그 주변에 Amount마리를 흩뿌림
		const FVector Center = GetEnemySpawnLocation(SwarmStraightRadius);
		
		// SwarmClass CDO에서 캡슐 반지름 읽어 간격 산출 (고정이지만 BP 값과 자동 동기화)
		float Spacing  = 68.f; // 폴백
		if (const APSCharacterEnemySwarm* CDO = SwarmClass.GetDefaultObject())
		{
			if (const UCapsuleComponent* Capsule = CDO->GetCapsuleComponent())
			{
				Spacing  = Capsule->GetScaledCapsuleRadius() * 2.f; // 지름 = 서로 안 겹침
			}
		}
		
		// 무리 전체가 공유할 방향: 중심 → 플레이어
		const APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
		FVector GroupDir = FVector(1.f, 0.f, 0.f);
		if (Player)
		{
			GroupDir = Player->GetActorLocation() - Center;
			GroupDir.Z = 0.f;
			GroupDir = GroupDir.GetSafeNormal();
		}
		
		// 해바라기(피보나치) 배치
		const float GoldenAngle = PI * (3.f - FMath::Sqrt(5.f)); // ≈ 2.399 rad
		for (int32 i = 0; i < FMapEvent.Amount; ++i)
		{
			const float R = Spacing * 0.5f * FMath::Sqrt(static_cast<float>(i)); // 바깥으로 퍼짐
			const float Angle = i * GoldenAngle;
			const FVector Offset(FMath::Cos(Angle) * R, FMath::Sin(Angle) * R, 0.f);
			SpawnOneSwarm(Center + Offset, FMapEvent.SwarmType, GroupDir, Data);
		}		
	}
	else //if(FMapEvent.SwarmType == ESwarmType::Encircle)
	{
		APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (!Player) return false;
		const FVector PlayerLoc = Player->GetActorLocation();

		const float RadiusX = EnemySpawnRadius / 3 * 2;  
		const float RadiusY = EnemySpawnRadius / 2;

		for (int32 i = 0; i < FMapEvent.Amount; ++i)
		{
			// Amount마리를 원 둘레에 균등 분할 + 약간의 랜덤
			const float Angle = (2.f * PI / FMapEvent.Amount) * i
							  + FMath::FRandRange(-0.1f, 0.1f);  // 살짝 흔들어 자연스럽게
			const FVector Offset(FMath::Cos(Angle) * RadiusX,
								 FMath::Sin(Angle) * RadiusY, 0.f);
			const FVector SpawnLoc = PlayerLoc + Offset;

			SpawnOneSwarm(SpawnLoc, FMapEvent.SwarmType, FVector::ZeroVector, Data);
		}
	}
		
	
	return true;
}

void UPSWaveSubsystem::SpawnOneSwarm(const FVector& Loc, ESwarmType SwarmType, const FVector& GroupDir,
	UPSPokemonData* Data)
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	if (!Pool) return;

	APSCharacterEnemySwarm* Swarm = Cast<APSCharacterEnemySwarm>(
		Pool->SpawnFromPool(SwarmClass, Loc, FRotator::ZeroRotator));
	check(Swarm != nullptr); // 터지면 풀 크기를 키울 것

	Swarm->Initialize(SwarmType, GroupDir);
	Swarm->FindComponentByClass<UPSStatComponent>()->Initialize(Data);
}

void UPSWaveSubsystem::SpawnBossesAndTreasure()
{
	for(const FPSBossSpawn& BossSpawn : Rows[CurrentRowIndex].BossesAndTreasure)
	{
        checkf(BossSpawn.BossClass, TEXT("SpawnBoss: BossClass null"));
		
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APSCharacterEnemyBoss* Boss = GetWorld()->SpawnActor<APSCharacterEnemyBoss>(
			BossSpawn.BossClass,
			GetEnemySpawnLocation(EnemySpawnRadius),
			FRotator::ZeroRotator,
			Params);
		check(Boss != nullptr);

		UPSStatComponent* Stat = Boss->FindComponentByClass<UPSStatComponent>();
		Stat->Initialize(BossSpawn.BossData);  // 플래그로 HP 배수 자동

		Boss->SetTreasure(BossSpawn.TreasureDrop);		
	}
}
