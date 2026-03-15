// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

#include "PSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class POKESURV_API APSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
private:
	void Move(const FInputActionValue& Value);
	
public:
	UPROPERTY(EditAnywhere, Category = "Input | PS")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input | PS")
	TObjectPtr<UInputAction> MoveAction;
	
	FVector LastMoveDirection = FVector(1.f, 0.f, 0.f); 
	
private:
	TObjectPtr<ACharacter> PlayerCharacter;
};
