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
			Projectile->Initialize(CurrentStats.Damage, CurrentStats.AreaMultiplier, CurrentStats.ProjectileSpeedMultiplier,
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
	if (NearestEnemyLocation.IsZero()) 
        return GetMoveDirection(); 
    
	return (NearestEnemyLocation - Owner->GetActorLocation()).GetSafeNormal();
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
