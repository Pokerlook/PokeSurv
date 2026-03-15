// Fill out your copyright notice in the Description page of Project Settings.

#include "PokeSurv/Player/PSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "InputAction.h"

void APSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerCharacter = Cast<ACharacter>(InPawn);
}

void APSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* InputSub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	InputSub->AddMappingContext(DefaultMappingContext, 0);
	
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APSPlayerController::Move);
}

void APSPlayerController::Move(const FInputActionValue& Value)
{
	if (!PlayerCharacter) return;

	FVector2D Input = Value.Get<FVector2D>();
	if (!Input.IsNearlyZero())
	{
		LastMoveDirection = FVector(Input.Y, Input.X, 0.f).GetSafeNormal();
	}
	PlayerCharacter->AddMovementInput(FVector(Input.Y, Input.X, 0.f));
	
	GEngine->AddOnScreenDebugMessage(123, 0.f, FColor::Yellow, 
		FString::Printf(TEXT("LastMoveDirection: X=%.2f Y=%.2f"), 
			LastMoveDirection.X, LastMoveDirection.Y));	// test
}
