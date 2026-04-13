// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PSCharacterPlayer.generated.h"

class UPSSkillComponent;
class APSPlayerController;
class UPSPlayerStatComponent;

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterPlayer : public APSCharacterBase
{
	GENERATED_BODY()
public:
	APSCharacterPlayer();
	
protected:
	virtual void BeginPlay() override;
	virtual void UpdateMoveVector(float DeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<APSPlayerController> PSPlayerController;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSPlayerStatComponent> PlayerStatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSSkillComponent> SkillComponent;
	
};
