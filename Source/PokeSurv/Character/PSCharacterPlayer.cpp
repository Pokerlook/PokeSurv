// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterPlayer.h"

#include "PokeSurv/Player/PSPlayerController.h"

void APSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
    PSPlayerController = Cast<APSPlayerController>(GetController());
}

void APSCharacterPlayer::UpdateMoveVector(float DeltaSeconds)
{
	FVector Velocity = GetVelocity();
    
	if (Velocity.Size() < 0.1f) return;
    if (!PSPlayerController) return;
    
	// LastMoveDirection은 입력 시점에 이미 정규화된 방향
	// X,Y 중 더 큰 축으로 4방향 결정
	FVector Dir = PSPlayerController->LastMoveDirection;
	if (FMath::Abs(Dir.Y) >= FMath::Abs(Dir.X))
	{
		MoveVector2D.X = Dir.Y > 0 ? 1 : -1;
		MoveVector2D.Y = 0;
	}
	else
	{
		MoveVector2D.Y = Dir.X > 0 ? 1 : -1;
		MoveVector2D.X = 0;
	}
}
