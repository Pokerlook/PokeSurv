// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Skill/PSSkillProjectile.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Combat/Projectile/PSProjectileBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"

void UPSSkillProjectile::Activate()
{
	RefillSpawnOffsets();
	
	if (AimMode == EProjectileAimMode::Random)
	{
		RefillFireDirections_RandomMode();
	}
	
    StartFireSequence();
}

void UPSSkillProjectile::FireOnce()
{
	if (!IsValid(Owner))
	{
		return;
	}
	
	const FVector SpawnLocation = Owner->GetActorLocation() + GetSpawnOffset();
    const FVector Direction = GetFireDirection(SpawnLocation);

	//DrawDebugSphere(GetWorld(), SpawnLocation, 2.f, 8, FColor::Cyan, false, 2.f);

	APSProjectileBase* Projectile = RequestProjectile(SpawnLocation, Direction.Rotation());

	if (IsValid(Projectile))
	{
		Projectile->Initialize(Owner,
			CalculateDamage(),
			CalculateAreaMultiplier(),
			CalculateProjectileSpeedMultiplier(),
			CalculateProjectilePierceCount(),
			GetKnockBack()
			);
		
		if (SkillData && SkillData->FireSound)
		{
			UGameplayStatics::PlaySound2D(this, SkillData->FireSound);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Projectile spawn failed. SkillData: %s, ProjectileClass: %s"),
			*GetName(),
			*GetNameSafe(SkillData),
			SkillData ? *GetNameSafe(SkillData->ProjectileClass) : TEXT("SkillData null"));
	}
	
}

FVector UPSSkillProjectile::GetFireDirection(const FVector& StartLocation)
{
	switch (AimMode)
	{
	case EProjectileAimMode::NearestEnemy:
		return GetNearestEnemyDirection(StartLocation);
        
	case EProjectileAimMode::MoveDirection:
		return GetMoveDirection();
        
	case EProjectileAimMode::Random:
		return GetNextFireDirection_RandomMode();
        
	default:
		return Owner->GetActorForwardVector();
	}
}

FVector UPSSkillProjectile::GetMoveDirection() const
{
	const APSPlayerController* PC = Cast<APSPlayerController>(
		Cast<ACharacter>(Owner)->GetController()
		);
    
	check(PC);
	return PC->LastMoveDirection;
}

FVector UPSSkillProjectile::GetNearestEnemyDirection(const FVector& StartLocation) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	for (const float Radius : {800.f, 1500.f})
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
		GetWorld()->OverlapMultiByObjectType(Overlaps, StartLocation,
			FQuat::Identity, ObjectParams, Sphere, QueryParams);

		// 탐색 범위 시각화
		/*DrawDebugSphere(GetWorld(), StartLocation, Radius, 16,
			Radius == 800.f ? FColor::Yellow : FColor::Orange,
			false, 1.1f);*/
		//
		
		AActor* Nearest = nullptr;
		float MinDist = FLT_MAX;

		for (auto& Overlap : Overlaps)
		{
			AActor* Actor = Overlap.GetActor();
			if (!Actor) continue;

			// 감지된 액터마다 점 표시
			DrawDebugPoint(GetWorld(), Actor->GetActorLocation(), 10.f, FColor::Red, false, 0.1f);
			
			float Dist = FVector::DistSquared(StartLocation, Actor->GetActorLocation());
			if (Dist < MinDist) { MinDist = Dist; Nearest = Actor; }
		}

		if (Nearest)
		{
			// 가장 가까운 적까지 선 표시
			//DrawDebugLine(GetWorld(), StartLocation, Nearest->GetActorLocation(), FColor::Green, false, 0.1f, 0, 2.f);
			//
			return (Nearest->GetActorLocation() - StartLocation).GetSafeNormal();
		}
	}

	return GetMoveDirection(); // fallback
}

APSProjectileBase* UPSSkillProjectile::RequestProjectile(FVector Location, FRotator Rotation)
{
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	check(PoolSubsystem);
    
	AActor* Actor = PoolSubsystem->SpawnFromPool(SkillData->ProjectileClass, Location, Rotation);
	return Cast<APSProjectileBase>(Actor);
}

FVector UPSSkillProjectile::GetSpawnOffset()
{
	if(RemainingSpawnOffsets.IsEmpty())
	{
		RefillSpawnOffsets();
	}

	const int RandomIndex = FMath::RandRange(0, RemainingSpawnOffsets.Num() - 1);
	const FVector Offset = RemainingSpawnOffsets[RandomIndex];
	RemainingSpawnOffsets.RemoveAtSwap (RandomIndex);

	return Offset;
}

void UPSSkillProjectile::RefillSpawnOffsets()
{
	RemainingSpawnOffsets.Empty();
	
	// 이동 방향에 수직인 벡터 계산
	const FVector MoveDir = GetMoveDirection();
    
	// XY 평면에서 90도 회전: (x, y) → (-y, x)
	const FVector Perpendicular(-MoveDir.Y, MoveDir.X, 0.f);
    
	constexpr float TotalLength = 80.f;       // 끝점 ↔ 끝점 거리
	constexpr int32 PointCount = 7;
	constexpr float HalfLength = TotalLength * 0.5f;
    
	// N개의 점을 [-HalfLength, +HalfLength] 구간에 균등 배치
	for (int32 i = 0; i < PointCount; i++)
	{
		// t = 0 ~ 1 사이를 균등하게
		const float t = static_cast<float>(i) / (PointCount - 1);
		// -HalfLength + t * TotalLength = -16, -10.67, -5.33, 0, +5.33, +10.67, +16
		const float Distance = -HalfLength + t * TotalLength;
        RemainingSpawnOffsets.Add(Perpendicular * Distance);
	}
}

void UPSSkillProjectile::RefillFireDirections_RandomMode()
{
	RemainingFireDirections_RandomMode.Empty();
    
	// 1. 캐릭터 기준 랜덤 베이스 방향
	const float BaseAngleDeg = FMath::RandRange(0.f, 360.f);
	const float BaseAngleRad = FMath::DegreesToRadians(BaseAngleDeg);
	const FVector BaseDirection(FMath::Cos(BaseAngleRad), FMath::Sin(BaseAngleRad), 0.f);
    
	// 2. 발사체 수만큼 콘 안에서 균등 분할
	const int32 ProjectileCount = CalculateProjectileCount();
	if (ProjectileCount <= 0) return;
    
	const float ConeWidth = RandomConeHalfAngleDegrees * 2.f;  // 콘 전체 각도
    
	for (int32 i = 0; i < ProjectileCount; i++)
	{
		// -1 ~ +1 사이 균등하게 분포
		// i=0이고 Count=1이면 t=0 (중앙)
		// i=0이고 Count>1이면 t=-1 (가장 왼쪽)
		const float t = (ProjectileCount == 1) 
			? 0.f 
			: (static_cast<float>(i) / (ProjectileCount - 1)) * 2.f - 1.f;
        
		// 베이스에서 회전 각도
		const float OffsetDeg = t * RandomConeHalfAngleDegrees;
		const float OffsetRad = FMath::DegreesToRadians(OffsetDeg);
        
		// BaseDirection을 OffsetDeg만큼 Z축 기준 회전
		// 2D 회전 행렬: (x*cos - y*sin, x*sin + y*cos)
		const float Cos = FMath::Cos(OffsetRad);
		const float Sin = FMath::Sin(OffsetRad);
		const FVector RotatedDir(
			BaseDirection.X * Cos - BaseDirection.Y * Sin,
			BaseDirection.X * Sin + BaseDirection.Y * Cos,
			0.f
		);
        
		RemainingFireDirections_RandomMode.Add(RotatedDir);
	}
}

FVector UPSSkillProjectile::GetNextFireDirection_RandomMode()
{
	if (RemainingFireDirections_RandomMode.IsEmpty())
	{
		// 안전장치: 비었으면 다시 채움 (이론상 안 일어남)
		RefillFireDirections_RandomMode();
	}
    
	// 큐 순서대로 (랜덤 X) — 균등 분할된 순서대로 발사
	const FVector Direction = RemainingFireDirections_RandomMode[0];
	RemainingFireDirections_RandomMode.RemoveAt(0);
	return Direction;
}
