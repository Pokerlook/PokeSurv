// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Actor/PSPoolableActor.h"

#include "PokeSurv/Game/PSPoolSubsystem.h"

void APSPoolableActor::OnSpawnFromPool_Implementation()
{
	SetActorHiddenInGame(false);
    SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void APSPoolableActor::OnReturnToPool_Implementation()
{
	SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
	SetActorEnableCollision(false);
}

bool APSPoolableActor::ShouldCull_Implementation()
{
	return false;
}

void APSPoolableActor::CheckCulling_Implementation(const FBox2D& ScreenBounds)
{
	const FVector Loc = GetActorLocation();
	const FVector2D Loc2D(Loc.X, Loc.Y);
    
	// 화면 영역 밖이면 풀로 반환
	if (!ScreenBounds.IsInside(Loc2D))
	{
		if (auto* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>())
		{
			Pool->ReturnToPool(this);
		}
	}
}
