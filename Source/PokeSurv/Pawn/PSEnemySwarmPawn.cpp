// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Pawn/PSEnemySwarmPawn.h"

#include "PokeSurv/Component/PSCrowdMovementComponent.h"

void APSEnemySwarmPawn::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool_Implementation();
	GetWorldTimerManager().ClearTimer(LifeTimer);
}

void APSEnemySwarmPawn::Initialize(ESwarmType InSwarmType, const FVector& InFlowDir)
{
	// OnSpawnFromPool -> Initialize 순서 (풀에서 꺼낸 뒤 호출)

	if (InSwarmType == ESwarmType::Straight)
	{
		// 직진 흐름: Flow 타입 + 방향 주입
		CrowdMovement->MoveType = ECrowdMoveType::Flow;

		FVector Dir = InFlowDir;
		Dir.Z = 0.f;
		Dir = Dir.GetSafeNormal();
		if (Dir.IsNearlyZero())
		{
			Dir = FVector(1.f, 0.f, 0.f); // 안전값
		}
		CrowdMovement->FlowDirection = Dir;

		GetWorldTimerManager().SetTimer(LifeTimer, this,
			&APSEnemySwarmPawn::RequestReturnToPool, StraightLifeSeconds, false);
	}
	else // Encircle: 일반 적과 동일하게 추적
	{
		CrowdMovement->MoveType = ECrowdMoveType::Chase;

		GetWorldTimerManager().SetTimer(LifeTimer, this,
			&APSEnemySwarmPawn::RequestReturnToPool, EncircleLifeSeconds, false);
	}
}
