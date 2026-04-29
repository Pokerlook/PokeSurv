// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Pickup/PSPickupBase.h"

#include "Components/SphereComponent.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"

APSPickupBase::APSPickupBase()
{
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
}

void APSPickupBase::ReturnToPool()
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
}

void APSPickupBase::SetValue_Implementation(float InValue)
{
	Value = InValue;
}
