// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PokeSurv/Interface/PSPoolable.h"
#include "PSCharacterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterEnemy : public APSCharacterBase, public IPSPoolable
{
	GENERATED_BODY()
public:
	virtual void PS_TakeDamage_Implementation(const FDamageResult  Damage) override;
	
	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;

	virtual bool ShouldCull_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void UpdateMoveVector(float DeltaSeconds) override;
	virtual void OnDeath_Implementation() override;
	
	virtual void HandleDrops();

	UFUNCTION(BlueprintCallable)
	void RequestReturnToPool();
	
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
