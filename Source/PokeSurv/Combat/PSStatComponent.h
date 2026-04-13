// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PokeSurv/Data/PokemonData.h"
#include "PSStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POKESURV_API UPSStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Initialize(UPokemonData* NewData);

protected:
	virtual void BeginPlay() override;
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Final")
	int32 MaxHP = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentHP = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TObjectPtr<UPokemonData> BaseData;
	
	int32 Attack;               // 실제 공격력
	int32 Defense;              // 실제 방어력 (PSPlayerStatComponent의 Defense 흡수)
	float Speed;                // 실제 이동속도
	float KnockbakMultiplier;

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnStatsChanged OnStatsChanged;
};
