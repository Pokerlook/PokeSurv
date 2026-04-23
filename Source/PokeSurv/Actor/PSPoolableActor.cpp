// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Actor/PSPoolableActor.h"

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
