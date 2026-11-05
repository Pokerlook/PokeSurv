// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSWaveSubsystem.h"

#include "PSPoolSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "PokeSurv/Component/PSStatComponent.h"
#include "PokeSurv/Pawn/PSEnemyBossPawn.h"
#include "PokeSurv/Pawn/PSEnemyPawn.h"
#include "PokeSurv/Pawn/PSEnemySwarmPawn.h"


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

void UPSWaveSubsystem::StartWave(const UDataTable* InWaveTable, const TSubclassOf<APSEnemyPawn>& InEnemyClass, const TSubclassOf<APSEnemySwarmPawn>& InSwarmClass)
{
	if (!GetWorld()) return;
	
	checkf(InWaveTable, TEXT("UPSWaveSubsystem::StartWave. WaveTable null"));
	checkf(InEnemyClass, TEXT("UPSWaveSubsystem::StartWave. EnemyClass null"));
	checkf(InSwarmClass, TEXT("UPSWaveSubsystem::StartWave. EnemySwarmClass null"));
	
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

	TryBeginFirstRow();  // 시작 즉시 1회 ApplyRow. 처음 시작 시 초기화 순서 때문.
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
	
	const FSpawnArea Area = MakeSpawnArea(SpawnMargin, EnemyClass);
	SpawnEnemy(Area);

	// 최소 수 까지 스폰
	int32 Alive = Pool->GetActiveCount(EnemyClass);
	//UE_LOG(LogTemp, Warning, TEXT("[Wave] Alive:%d / Min:%d"), Alive, Rows[CurrentRowIndex].EnemyMinimum);
	
	while( Alive < Rows[CurrentRowIndex].EnemyMinimum)
	{
		if (!SpawnEnemy(Area)) break;		// pool 고갈
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

void UPSWaveSubsystem::TryBeginFirstRow()
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();

	if (!Pool->GetScreenWorldBounds().bIsValid)
	{
		// 아직 준비 안 됨 → 다음 틱에 재시도
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UPSWaveSubsystem::TryBeginFirstRow);
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(RowCheckTimer, this, &UPSWaveSubsystem::RowCheck, 60.f, true);
	ApplyRow();
}

bool UPSWaveSubsystem::SpawnEnemy(const FSpawnArea& Area)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	const FPSWaveRow& Row = Rows[CurrentRowIndex];
	if (Row.Enemies.Num() == 0) return false;  // % 0, empty.
	
	UPSPokemonData* Data = Row.Enemies[NextEnemyIndex];
	//if (!Data) return false;		Data null 이면 터지도록 주석처리
	NextEnemyIndex = (NextEnemyIndex + 1) % Row.Enemies.Num();
	
	UPSPoolSubsystem* Pool = World->GetSubsystem<UPSPoolSubsystem>();
	if (!Pool) return false;

	APSEnemyPawn* Enemy = Cast<APSEnemyPawn>(
		Pool->SpawnFromPool(EnemyClass, RandomOnCircle(Area), FRotator::ZeroRotator));
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
		// 간격 (캡슐 지름)
		float Spacing = 68.f; // 폴백
		if (APSEnemySwarmPawn* CDO = SwarmClass.GetDefaultObject())
		{
			if (const UCapsuleComponent* Capsule = CDO->GetCapsuleComponent())
			{
				Spacing = Capsule->GetScaledCapsuleRadius() * 2.f;
			}
		}
		
		// 해바라기 최외곽 반경 → 대형 전체가 화면 밖에서 시작하도록 Margin에 더함
		const float FormationRadius =
			Spacing * 0.5f * FMath::Sqrt(static_cast<float>(FMath::Max(FMapEvent.Amount - 1, 0)));

		const FSpawnArea Area = MakeSpawnArea(SpawnMargin + FormationRadius, SwarmClass);
		const FVector Center = RandomOnCircle(Area);
		
		// 무리 전체가 공유할 방향: 화면 중심(플레이어)
		FVector GroupDir(Area.Center.X - Center.X, Area.Center.Y - Center.Y, 0.f);
		GroupDir = GroupDir.GetSafeNormal();
		
		if (GroupDir.IsNearlyZero()) GroupDir = FVector(1.f, 0.f, 0.f);
		
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
		const FSpawnArea Area = MakeSpawnArea(0.f, SwarmClass);  // 화면 반크기 그대로
		const float RadiusX = Area.W * EncircleScale;
		const float RadiusY = Area.H * EncircleScale;

		for (int32 i = 0; i < FMapEvent.Amount; ++i)
		{
			// Amount마리를 원 둘레에 균등 분할 + 약간의 랜덤
			const float Angle = (2.f * PI / FMapEvent.Amount) * i
			+ FMath::FRandRange(-0.1f, 0.1f);  // 살짝 흔들어 자연스럽게
			
			const FVector SpawnLoc(Area.Center.X + FMath::Cos(Angle) * RadiusX,
				Area.Center.Y + FMath::Sin(Angle) * RadiusY, Area.Z);

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

	APSEnemySwarmPawn* Swarm = Cast<APSEnemySwarmPawn>(
		Pool->SpawnFromPool(SwarmClass, Loc, FRotator::ZeroRotator));
	check(Swarm != nullptr); // 터지면 풀 크기를 키울 것

	Swarm->Initialize(SwarmType, GroupDir);
	Swarm->FindComponentByClass<UPSStatComponent>()->Initialize(Data);
}

void UPSWaveSubsystem::SpawnBossesAndTreasure()
{
	const TArray<FPSBossSpawn>& Bosses = Rows[CurrentRowIndex].BossesAndTreasure;
	if (Bosses.Num() == 0) return;
	
	const FSpawnArea Area = MakeSpawnArea(SpawnMargin, EnemyClass);  // 일반 적과 같은 높이

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for(const FPSBossSpawn& BossSpawn : Rows[CurrentRowIndex].BossesAndTreasure)
	{
        checkf(BossSpawn.BossClass, TEXT("SpawnBoss: BossClass null"));
		
		APSEnemyBossPawn* Boss = GetWorld()->SpawnActor<APSEnemyBossPawn>(
			BossSpawn.BossClass,
			RandomOnCircle(Area),
			FRotator::ZeroRotator,
			Params);
		check(Boss != nullptr);

		UPSStatComponent* Stat = Boss->FindComponentByClass<UPSStatComponent>();
		Stat->Initialize(BossSpawn.BossData);  // 플래그로 HP 배수 자동

		Boss->SetTreasure(BossSpawn.TreasureDrop);		
	}
}

float UPSWaveSubsystem::GetFloorZ() const
{
	const APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player) return 0.f;

	const FVector PlayerLoc = Player->GetActorLocation();

	// 폴백: 플레이어 캡슐 하단
	float FloorZ = PlayerLoc.Z;
	if (const ACharacter* Char = Cast<ACharacter>(Player))
	{
		FloorZ -= Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	// 아래로 트레이스해서 실제 바닥 찾기 (공중에 떠 있어도 OK)
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);

	const FVector Start = PlayerLoc;
	const FVector End   = PlayerLoc - FVector(0.f, 0.f, 2000.f);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
	{
		FloorZ = Hit.ImpactPoint.Z;
	}
	return FloorZ;
}

float UPSWaveSubsystem::GetSpawnZ(TSubclassOf<APSEnemyPawn> InClass) const
{
	float HalfHeight = 34.f; 
	if (InClass)
	{
		if (const APSEnemyPawn* CDO = InClass.GetDefaultObject())
		{
			if (const UCapsuleComponent* Capsule = CDO->GetCapsuleComponent())
			{
				HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
			}
		}
	}
	return GetFloorZ() + HalfHeight;
}

FSpawnArea UPSWaveSubsystem::MakeSpawnArea(float Margin, TSubclassOf<APSEnemyPawn> InClass)
{
	const FBox2D Bounds = GetWorld()->GetSubsystem<UPSPoolSubsystem>()->GetScreenWorldBounds();
	ensureMsgf(Bounds.bIsValid, TEXT("MakeSpawnArea: ScreenWorldBounds 무효 — TryBeginFirstRow 이전 호출?"));
	
	const FVector2D Extent = Bounds.GetExtent();
	
	FSpawnArea A;
	A.Center = Bounds.GetCenter();
	A.W = Extent.X + Margin;
	A.H = Extent.Y + Margin;
	A.Radius = Extent.Size() + Margin; 
	A.Z = GetSpawnZ(InClass);
	return A;
}

FVector UPSWaveSubsystem::RandomOnCircle(const FSpawnArea& A)
{
	const float Angle = FMath::FRandRange(0.f, 2.f * PI);
	return FVector(A.Center.X + FMath::Cos(Angle) * A.Radius,
				   A.Center.Y + FMath::Sin(Angle) * A.Radius,
				   A.Z);
}