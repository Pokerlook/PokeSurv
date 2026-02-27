// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterPlayer.h"

void APSCharacterPlayer::UpdateMoveVector(float DeltaSeconds)
{
	FVector Velocity = GetVelocity();
    
	if (Velocity.Size() < 0.1f) return;
    
	if (FMath::Abs(Velocity.X) >= FMath::Abs(Velocity.Y))
	{
		MoveVector2D.Y = Velocity.X > 0 ? 1 : -1;
		MoveVector2D.X = 0;
	}
	else
	{
		MoveVector2D.X = Velocity.Y > 0 ? 1 : -1;
		MoveVector2D.Y = 0;
	}
}
