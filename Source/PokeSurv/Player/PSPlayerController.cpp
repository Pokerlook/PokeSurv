// Fill out your copyright notice in the Description page of Project Settings.

#include "PokeSurv/Player/PSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PokeSurv/UI/DamageTextWidget.h"

void APSPlayerController::ShowDamageNumber(FDamageResult DamageResult, AActor* TargetActor)
{
	if (DamageTextPool.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] DamageTextPool is empty."),
			*GetName());
		return;
	}
	
	UDamageTextWidget* Widget = DamageTextPool.Pop();
	if (!Widget)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] DamageTextWidget popped from pool is null."),
			*GetName());
		return;
	}

	//FVector2D ScreenPos;
	//ProjectWorldLocationToScreen(TargetActor->GetActorLocation(), ScreenPos);
	//Widget->SetPositionInViewport(ScreenPos);
	Widget->WorldLocation = TargetActor->GetActorLocation();
	Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
	Widget->SetDamageText(DamageResult);
}

void APSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// BeginPlay에서 풀 초기화
	for (int32 i = 0; i < DamageTextPoolSize; i++)
	{
		UDamageTextWidget* Widget = CreateWidget<UDamageTextWidget>(this, DamageTextClass.Get());
		DamageTextPool.Add(Widget);
		Widget->OnDamageTextFinished.AddDynamic(this, &APSPlayerController::OnDamageTextFinished);
		Widget->AddToViewport();
		Widget->SetVisibility(ESlateVisibility::Collapsed);
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

void APSPlayerController::OnDamageTextFinished(UDamageTextWidget* Widget)
{
	Widget->SetVisibility(ESlateVisibility::Collapsed);
	DamageTextPool.Add(Widget);
}
