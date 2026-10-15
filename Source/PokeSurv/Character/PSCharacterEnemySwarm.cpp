// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterEnemySwarm.h"

#include "Components/CapsuleComponent.h"

void APSCharacterEnemySwarm::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PushAccum += DeltaSeconds;
	if (PushAccum >= PushInterval)
	{
		PushAccum = 0.f;
		PushBlockingEnemies();
	}
}

void APSCharacterEnemySwarm::PushBlockingEnemies()
{
	const FVector MoveDir = GetMovementDirection();
	if (MoveDir.IsNearlyZero()) return;

	const float Radius = GetCapsuleComponent()->GetScaledCapsuleRadius() * PushRadiusScale;

	TArray<FOverlapResult> Overlaps;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(),
		FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), Sphere, Params);

	const FVector MyLoc = GetActorLocation();

	for (const FOverlapResult& O : Overlaps)
	{
		AActor* Other = O.GetActor();
		if (!Other) continue;

		// 일반 Enemy만 대상 (Swarm끼리 제외, Player 제외)
		if (!Other->ActorHasTag("Enemy")) continue;
		if (Other->IsA(APSCharacterEnemySwarm::StaticClass())) continue;

		ACharacter* OtherChar = Cast<ACharacter>(Other);
		if (!OtherChar) continue;

		// 순수 계산으로 밀 방향 구하기 (앞쪽 아니면 ZeroVector)
		const FVector PushDir = CalcPushDir(MyLoc, MoveDir, Other->GetActorLocation());
		if (PushDir.IsNearlyZero()) continue;

		OtherChar->LaunchCharacter(PushDir * PushForce, true, false);
	}
}

FVector APSCharacterEnemySwarm::CalcPushDir(const FVector& MyLoc, const FVector& MoveDir, const FVector& OtherLoc)
{
	FVector ToOther = OtherLoc - MyLoc;
	ToOther.Z = 0.f;

	// 진행방향 앞쪽에 있는 놈만 밂 (내적 > 0)
	if (FVector::DotProduct(MoveDir, ToOther.GetSafeNormal()) <= 0.f)
		return FVector::ZeroVector;

	// 진행방향 기준 상대가 왼쪽이면 더 왼쪽으로, 오른쪽이면 더 오른쪽으로 비켜냄
	const FVector Cross = FVector::CrossProduct(MoveDir, ToOther);
	return (Cross.Z >= 0.f)
		? FVector::CrossProduct(MoveDir, FVector::UpVector).GetSafeNormal()
		: FVector::CrossProduct(FVector::UpVector, MoveDir).GetSafeNormal();
}

void APSCharacterEnemySwarm::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool_Implementation();

	// 시작 오프셋을 랜덤하게 → 모든 Swarm이 같은 프레임에 쿼리하는 것 방지
	PushAccum = FMath::FRandRange(0.f, PushInterval);
}

void APSCharacterEnemySwarm::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool_Implementation();

	StraightDir = FVector::ZeroVector;
    GetWorldTimerManager().ClearTimer(LifeTimer); 
}

void APSCharacterEnemySwarm::Initialize(ESwarmType InSwarmType, const FVector& InStraightDir)
{
	SwarmType = InSwarmType;
	// OnSpawnFromPool -> Initialize
	
	if (SwarmType == ESwarmType::Straight)
	{
		StraightDir = InStraightDir.GetSafeNormal();

		if (StraightDir.IsNearlyZero())
		{
			StraightDir = FVector(1.f, 0.f, 0.f); // 안전값
		}
		
		GetWorldTimerManager().SetTimer(LifeTimer, this,
			&APSCharacterEnemySwarm::RequestReturnToPool, 15.f, false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(LifeTimer, this,
			&APSCharacterEnemySwarm::RequestReturnToPool, 30.f, false);
	}
}

FVector APSCharacterEnemySwarm::GetMovementDirection() const
{
	if (SwarmType == ESwarmType::Straight)
	{
		return StraightDir; // 고정 직진
	}
	
	// Encircle: 부모(플레이어 추적) 그대로
	return Super::GetMovementDirection();
}
