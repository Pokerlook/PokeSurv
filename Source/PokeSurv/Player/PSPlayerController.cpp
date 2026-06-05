// Fill out your copyright notice in the Description page of Project Settings.

#include "PokeSurv/Player/PSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PokeSurv/UI/PSDamageTextWidget.h"

void APSPlayerController::ShowDamageNumber(FDamageResult DamageResult, AActor* TargetActor)
{
	if (DamageTextPool.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] DamageTextPool is empty."),
			*GetName());
		return;
	}
	
	UPSDamageTextWidget* Widget = DamageTextPool.Pop();
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
		UPSDamageTextWidget* Widget = CreateWidget<UPSDamageTextWidget>(this, DamageTextClass.Get());
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
	
	if (!Input.IsNearlyZero())  // moving
	{
        // 8방향 정규화 (이동 중인 동안만 갱신)
		LastMoveDirection = FVector(Input.Y, Input.X, 0.f).GetSafeNormal();
        
		// 좌우 입력 있을 때만 bLastMovedRight 갱신
		if (!FMath::IsNearlyZero(Input.X))
		{
			bLastMovedRight = (Input.X > 0);
		}
		// Input.X == 0이면 (위/아래만 입력) 이전 값 유지
	}

	PlayerCharacter->AddMovementInput(FVector(Input.Y, Input.X, 0.f));
}

void APSPlayerController::OnDamageTextFinished(UPSDamageTextWidget* Widget)
{
	Widget->SetVisibility(ESlateVisibility::Collapsed);
	DamageTextPool.Add(Widget);
}
