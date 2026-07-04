// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimSequences/PaperZDAnimSequence.h"
#include "PSPokemonData.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API UPSPokemonData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FName PokemonName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	TObjectPtr<UPaperZDAnimSequence>  AnimSequence;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	int32 Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	int32 Power = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	float Speed = 100.f;
	
	/* Knockback taken multiplier.
	* Base: 기본 넉백 배율
	* Max: Garlic/Soul Eater 디버프 누적 시 최대값 (bDebuffResistance = true면 무시됨)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	FVector2D KnockbackMultiplier = FVector2D(1.f, 1.f); // (Base, Max)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	float XP = 1;

	// true면 Health × 플레이어레벨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseStats")
	bool bScaleHPByPlayerLevel = false;  
};
