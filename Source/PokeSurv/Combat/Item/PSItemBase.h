// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSItemBase.generated.h"

class UPSItemData;
class UPSPlayerStatComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class POKESURV_API UPSItemBase : public UObject
{
	GENERATED_BODY()
public:
	
	void InitItem(AActor* InOwner);
	void LevelUp();

protected:
	UPROPERTY() // 필요없을지도 모름
	TObjectPtr<AActor> Owner;
	
	UPROPERTY()
	UPSPlayerStatComponent* PlayerStatComp;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemLevel = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS")
	TObjectPtr<UPSItemData> ItemData;
};
