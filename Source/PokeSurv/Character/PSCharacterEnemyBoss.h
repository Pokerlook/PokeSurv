// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterEnemy.h"
#include "PSCharacterEnemyBoss.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterEnemyBoss : public APSCharacterEnemy
{
	GENERATED_BODY()

public:
	void SetTreasure(const TSubclassOf<APSPickupBase>& InTreasureClass);

protected:
	virtual void HandleDrops() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "PS")
	TSubclassOf<APSPickupBase> TreasureChestClass;
};
