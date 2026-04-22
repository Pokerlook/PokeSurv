// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PSHealthWidgetComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float, CurrentHP, float, MaxHP);

/**
 * 
 */
UCLASS()
class POKESURV_API UPSHealthWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void UpdateHP(float CurrentHP, float MaxHP) const;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnHPChanged OnHPChanged;
	
};
