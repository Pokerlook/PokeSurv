// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterEnemy.h"

#include "Kismet/GameplayStatics.h"

void APSCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();
	TargetCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void APSCharacterEnemy::UpdateMoveVector(float DeltaSeconds)
{
	FVector ToPlayer = TargetCharacter->GetActorLocation() - GetActorLocation();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(ToPlayer.Y, ToPlayer.X));

	if      (Angle >= -45.f  && Angle < 45.f)   MoveVector2D = FVector2D(0, 1);
	else if (Angle >= 45.f   && Angle < 135.f)  MoveVector2D = FVector2D(1, 0);
	else if (Angle >= -135.f && Angle < -45.f)  MoveVector2D = FVector2D(-1, 0);
	else                                         MoveVector2D = FVector2D(0, -1);
}
