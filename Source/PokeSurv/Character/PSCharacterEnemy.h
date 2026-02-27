// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PSCharacterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterEnemy : public APSCharacterBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void UpdateMoveVector(float DeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter;
};
