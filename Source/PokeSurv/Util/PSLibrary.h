// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PokeSurv/PSTypes.h"
#include "PSLibrary.generated.h"

class UPSSkillBase;
class UPSItemBase;
class UPSSkillComponent;


DECLARE_LOG_CATEGORY_EXTERN(LogPSHit, Log, All);

namespace PSHitDebug
{
	bool IsOn();
	FString DescribeCol(const UPrimitiveComponent* C, const UPrimitiveComponent* Other);
}

#define PS_HITLOG(Fmt, ...) \
if (PSHitDebug::IsOn()) { UE_LOG(LogPSHit, Warning, Fmt, ##__VA_ARGS__); }

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
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PS|Utility", meta = (WorldContext = "WorldContextObject"))
    static int32 GetElapsedSeconds(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "PS|Utility", meta = (WorldContext = "WorldContextObject"))
	static void DebugPlayerSkillsAndItems(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "PS|Utility", meta = (WorldContext = "WorldContextObject"))
	static void DebugPlayerStat(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PS|Utility")
	static bool IsAlive(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "PS|Utility")
	static int32 GetSkillRarity(const TSubclassOf<UPSSkillBase>& SkillClass);

	UFUNCTION(BlueprintPure, Category = "PS|Utility")
	static int32 GetItemRarity(const TSubclassOf<UPSItemBase>& ItemClass);
	
	UFUNCTION(BlueprintPure, Category = "PS|Utility")
	static FString GetOptionName(const FPSLevelUpOption& Option);

	UFUNCTION(BlueprintPure, Category = "PS|Utility")
	static UTexture2D* GetOptionIcon(const FPSLevelUpOption& Option);

	UFUNCTION(BlueprintPure, Category = "PS|Utility")
	static FText GetOptionBaseDescription(const FPSLevelUpOption& Option);
	
	UFUNCTION(BlueprintPure, Category = "PS|Utility", meta = (WorldContext = "WorldContextObject"))
	static UPSSkillComponent* GetPlayerSkillComponent(const UObject* WorldContextObject);
};
