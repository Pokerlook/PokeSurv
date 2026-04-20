// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSTypes.generated.h"

// 여러 곳에서 공통적으로 쓸만한 enum / struct...

USTRUCT(BlueprintType)
struct FDamageResult
{
	GENERATED_BODY()
    
	UPROPERTY(BlueprintReadWrite)
	float FinalDamage = 0.f;
	
    UPROPERTY(BlueprintReadWrite)
	bool bIsCritical = false;
};