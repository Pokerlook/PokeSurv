// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PSGameModeBase.generated.h"

class APSPickupBase;
/**
 * 
 */
UCLASS

()
class POKESURV_API APSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SpawnExpGem(FVector Location, float XPValue);
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APSPickupBase> ExpGemClass;
	
	UPROPERTY()
	float PendingXP = 0.f;
};
