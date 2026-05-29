// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Combat/Skill/PSSkillBase.h"
#include "PSSkillSummon.generated.h"

class APSSummonBase;

/**
 * 
 */
UCLASS()
class POKESURV_API UPSSkillSummon : public UPSSkillBase
{
	GENERATED_BODY()
	
protected:
	
	virtual void Activate() override;
	virtual void FireOnce() override;

private:
	
	APSSummonBase* RequestSummon(FVector Location, FRotator Rotation);

	FVector GetSpawnLocation();
	FRotator  GetSpawnRotation();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PS")
	float SpawnStackSpacing;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PS")
	ESummonSpawnPattern  SummonSpawnPattern;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PS")
	ESummonBehavior SummonBehavior;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PS")
	ESummonDamageMode SummonDamageMode;
	
private:
	bool bNextSpawnRight = false;  // 다음 발사 좌/우
	float CurrentSpawnStackOffset = 0;
};
