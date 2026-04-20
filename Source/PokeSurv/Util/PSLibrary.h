// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PokeSurv/PSTypes.h"
#include "PSLibrary.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API UPSLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void ApplyDamage(AActor* Target, FDamageResult Damage, float KnockBackMultiplier, AActor* DamageCauser);

};
