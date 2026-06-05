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
	UFUNCTION(BlueprintCallable, Category = "PS|Utility")
	static void ApplyDamage(AActor* Target, FDamageResult Damage, float KnockBackMultiplier, AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PS|Utility")
	static bool IsHostileTo(AActor* A, AActor* B);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PS|Utility")
	static FText FormatTime_MS(int32 Seconds);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PS|Utility")
    static int32 GetElapsedSeconds(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "PS|Utility")
	static void DebugPlayerSkillsAndItems(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "PS|Utility")
	static void DebugPlayerStat(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PS|Utility")
	static bool IsAlive(AActor* Actor);
};
