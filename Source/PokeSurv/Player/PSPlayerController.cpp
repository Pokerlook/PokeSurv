// Fill out your copyright notice in the Description page of Project Settings.

#include "PokeSurv/Player/PSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PokeSurv/UI/DamageTextComponent.h"

void APSPlayerController::ShowDamageNumber(FDamageResult DamageResult, AActor* TargetActor)
{
	if (IsValid(TargetActor) && !TargetActor->IsHidden() && DamageTextComponentClass)	// hidden이면 풀에 잠들어 있는 것
	{
		// 이걸 나중에 풀에서 가져오는 걸로 바꾸면 됨
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetActor, DamageTextComponentClass);
		
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageResult);	// BP 에서 풀에 반환하도록 함수 마지막에 설정하면 됨
	}
}

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
	if (!Input.IsNearlyZero())	// moving
	{
		LastMoveDirection = FVector(Input.Y, Input.X, 0.f).GetSafeNormal();
	}
	else    // stop
	{
		if (FMath::Abs(Input.X) >= FMath::Abs(Input.Y))
		{
			LastMoveDirection = FVector(0.f, Input.X > 0 ? 1.f : -1.f, 0.f);
		}
		else
		{
			LastMoveDirection = FVector(Input.Y > 0 ? 1.f : -1.f, 0.f, 0.f);
		}
	}
	PlayerCharacter->AddMovementInput(FVector(Input.Y, Input.X, 0.f));
}
