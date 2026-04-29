// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSGameModeBase.h"

#include "PSPoolSubsystem.h"
#include "PokeSurv/Combat/Pickup/PSPickupBase.h"

void APSGameModeBase::SpawnExpGem(FVector Location, float XPValue)
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	APSPickupBase* Gem = Cast<APSPickupBase>(
		Pool->SpawnFromPool(ExpGemClass, FVector(Location.X, Location.Y, 0), FRotator::ZeroRotator)
	);

	if (Gem)
	{
		float Total = XPValue + PendingXP;
		PendingXP = 0.f;
		Gem->SetValue(Total);
	}
	else
	{
		PendingXP += XPValue;
	}
}

void APSGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	if (Pool)
	{
		Pool->InitPool(TSubclassOf<APSPoolableActor>(ExpGemClass), 400);
	}
}
