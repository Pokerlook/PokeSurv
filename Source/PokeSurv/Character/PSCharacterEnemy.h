// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PSCharacterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterEnemy : public APSCharacterBase
{
	GENERATED_BODY()
public:
	virtual void PS_TakeDamage_Implementation(const FDamageResult  Damage) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void UpdateMoveVector(float DeltaSeconds) override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ContactDamageTrace();

private:
	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter;
	
	FTimerHandle ContactDamageTimer;
	float ContactDamageInterval = 0.5f;
};
