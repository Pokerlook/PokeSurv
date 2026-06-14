// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"
#include "PSItemData.generated.h"

/**
 * 
*/
USTRUCT(BlueprintType)
struct FItemStatEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerStatType StatType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Value = 0.f;
};


UCLASS()
class POKESURV_API UPSItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ItemName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int MaxLevel =5;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Rarity = 1;

	// 정수. ProjectileCount, Defense, Revive, Reroll, Skip, Banish
	// 소숫점. Recovery
	// 나머지. 백분율, 나누기 100 할 것.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FItemStatEntry> StatPerLevel; // 레벨업마다 이 값들을 AddStat
};
