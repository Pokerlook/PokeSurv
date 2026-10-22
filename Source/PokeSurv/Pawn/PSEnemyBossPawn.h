// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Pawn/PSEnemyPawn.h"
#include "PSEnemyBossPawn.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSEnemyBossPawn : public APSEnemyPawn
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
