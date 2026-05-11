// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PSGameModeBase.generated.h"

class APSPickupBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillCountChanged, int32, KillCount);

/**
 * 
 */
UCLASS()
class POKESURV_API APSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SpawnExpGem(FVector Location, float XPValue);
	
	void AddKillCount();
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnKillCountChanged OnKillCountChanged;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APSPickupBase> ExpGemClass;
	
	UPROPERTY()
	float PendingXP = 0.f;
	
	UPROPERTY()
	int32 KillCount = 0;
};
