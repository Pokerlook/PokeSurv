// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PSCharacterPlayer.generated.h"

class APSPlayerController;

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterPlayer : public APSCharacterBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void UpdateMoveVector(float DeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<APSPlayerController> PSPlayerController;
	
};
