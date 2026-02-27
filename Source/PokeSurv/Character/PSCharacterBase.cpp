// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterBase.h"
#include "PaperFlipbookComponent.h"

void APSCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    
	MoveSpeed = GetVelocity().Size();

	UpdateMoveVector(DeltaSeconds);
}
