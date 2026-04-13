// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Skill/PSSkillProjectile.h"

#include "GameFramework/Character.h"
#include "PokeSurv/Combat/Actor/PSProjectileBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"

void UPSSkillProjectile::Activate()
{
	FVector SpawnLocation = Owner->GetActorLocation();

	for (int32 i = 0; i < CurrentStats.ProjectileCount; i++)
	{
		FVector Direction = GetFireDirection();
		APSProjectileBase* Projectile = RequestProjectile(SpawnLocation, Direction.Rotation());
		if (IsValid(Projectile))
		{
			Projectile->Initialize(Owner, CalculateDamage(), CurrentStats.AreaMultiplier, CurrentStats.ProjectileSpeedMultiplier,
				CurrentStats.PierceCount, CurrentStats.Knockback);
		}
	}
}

FVector UPSSkillProjectile::GetFireDirection()
{
	switch (AimMode)
	{
	case EProjectileAimMode::NearestEnemy:
		return GetNearestEnemyDirection();
        
	case EProjectileAimMode::MoveDirection:
		return GetMoveDirection();
        
	case EProjectileAimMode::Random:
		{
			float RandomAngle = FMath::RandRange(0.f, 360.f);
			return FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)), 
						   FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 
						   0.f);
		}
        
	default:
		return Owner->GetActorForwardVector();
	}
}

FVector UPSSkillProjectile::GetNearestEnemyDirection()
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FVector OwnerLocation = Owner->GetActorLocation();

	for (float Radius : {800.f, 1500.f})
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
		GetWorld()->OverlapMultiByObjectType(Overlaps, OwnerLocation,
			FQuat::Identity, ObjectParams, Sphere, QueryParams);

		// 탐색 범위 시각화
		DrawDebugSphere(GetWorld(), OwnerLocation, Radius, 16,
			Radius == 800.f ? FColor::Yellow : FColor::Orange,
			false, 1.1f);
		//
		
		AActor* Nearest = nullptr;
		float MinDist = FLT_MAX;

		for (auto& Overlap : Overlaps)
		{
			AActor* Actor = Overlap.GetActor();
			if (!Actor) continue;

			// 감지된 액터마다 점 표시
			DrawDebugPoint(GetWorld(), Actor->GetActorLocation(),
				10.f, FColor::Red, false, 0.1f);
			//
			
			float Dist = FVector::DistSquared(OwnerLocation, Actor->GetActorLocation());
			if (Dist < MinDist) { MinDist = Dist; Nearest = Actor; }
		}

		if (Nearest)
		{
			// 가장 가까운 적까지 선 표시
			DrawDebugLine(GetWorld(), OwnerLocation, Nearest->GetActorLocation(),
				FColor::Green, false, 0.1f, 0, 2.f);
			//
			return (Nearest->GetActorLocation() - OwnerLocation).GetSafeNormal();
		}
	}

	return GetMoveDirection(); // fallback

}

FVector UPSSkillProjectile::GetMoveDirection()
{
	APSPlayerController* PC = Cast<APSPlayerController>(
		Cast<ACharacter>(Owner)->GetController());
    
	check(PC);
	return PC->LastMoveDirection;
}

APSProjectileBase* UPSSkillProjectile::RequestProjectile(FVector Location, FRotator Rotation)
{
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	check(PoolSubsystem);
    
	AActor* Actor = PoolSubsystem->SpawnFromPool(SkillData->ProjectileClass, Location, Rotation);
	return Cast<APSProjectileBase>(Actor);
}
