// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PokeSurv/PSTypes.h"
#include "PSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UPSDamageTextWidget;

/**
 * 
 */
UCLASS()
class POKESURV_API APSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void ShowDamageNumber(FDamageResult DamageResult, AActor* TargetActor);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	
private:
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void OnDamageTextFinished(UPSDamageTextWidget* Widget);
	
public:
	UPROPERTY(EditAnywhere, Category = "Input | PS")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input | PS")
	TObjectPtr<UInputAction> MoveAction;
	
	FVector LastMoveDirection = FVector(1.f, 0.f, 0.f); 
	
	bool bLastMovedRight = false;  // 마지막 수평 이동이 오른쪽?
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> PlayerCharacter;

	UPROPERTY()
	TArray<TObjectPtr<UPSDamageTextWidget>> DamageTextPool;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPSDamageTextWidget> DamageTextClass;

	int32 DamageTextPoolSize = 200;
};
