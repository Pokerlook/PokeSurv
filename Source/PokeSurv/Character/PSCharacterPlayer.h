// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PSCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterPlayer : public APSCharacterBase
{
	GENERATED_BODY()
protected:
	virtual void UpdateMoveVector(float DeltaSeconds) override;

};
