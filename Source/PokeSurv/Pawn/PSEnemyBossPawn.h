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

	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
protected:
	virtual void HandleDrops() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "PS")
	TSubclassOf<APSPickupBase> TreasureChestClass;
};
