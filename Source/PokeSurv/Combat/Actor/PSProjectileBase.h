// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PSPoolableActor.h"
#include "PokeSurv/PSTypes.h"
#include "PSProjectileBase.generated.h"

class USphereComponent;
class UPaperFlipbookComponent;
class UProjectileMovementComponent;

UCLASS()
class POKESURV_API APSProjectileBase : public APSPoolableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSProjectileBase();
	
	void Initialize(AActor* InOwner, FDamageResult  InDamage, float InAreaMultiplier, float InSpeedMultiplier, int32 InPierceCount, float InKnockBackMultiplier);

	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> CollisionSphere;	// root

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipbookComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PSProjectile)
	float ProjectileLifeSpan  = 5.f;
	
private:
	FTimerHandle LifeSpanTimer;
	TArray<TWeakObjectPtr<AActor>> HitActors;	//풀에 반환할 때 비운
	
	float DefaultSpeed = 0.f;
	float DefaultRadius = 0.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	
	FDamageResult FinalDamage;
	int32 PierceCount = 0;
	int32 BounceCount = 0;  // 0이면 바운스 없음
	float KnockBackMultiplier = 0.f;
};
