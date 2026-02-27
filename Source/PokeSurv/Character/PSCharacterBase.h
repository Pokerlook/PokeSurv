// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h" 
#include "PokeSurv/PSTypes.h"
#include "PSCharacterBase.generated.h"

UCLASS()
class POKESURV_API APSCharacterBase : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void UpdateMoveVector(float DeltaSeconds) PURE_VIRTUAL(APSCharacterBase::UpdateMoveVector,);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FVector2D MoveVector2D = FVector2D(0, -1);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float MoveSpeed;	
};
