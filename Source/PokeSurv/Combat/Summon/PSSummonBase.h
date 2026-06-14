// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/PSTypes.h"
#include "PokeSurv/Actor/PSPoolableActor.h"
#include "PSSummonBase.generated.h"

class UBoxComponent;
class UPaperFlipbookComponent;


/**
 * 
 */
UCLASS()
class POKESURV_API APSSummonBase : public APSPoolableActor
{
	GENERATED_BODY()
	
public:
	APSSummonBase();

	virtual void Tick(float DeltaSeconds) override;
	
	void Initialize(AActor* InOwner, ESummonBehavior InSummonBehavior, ESummonDamageMode InSummonDamageMode, 
		FDamageResult InDamage, float InAreaMultiplier, float InHitboxDelay, float InDuration, float InKnockBackMultiplier);

	void UpdateStats(FDamageResult InDamage, float InAreaMultiplier, float InHitboxDelay, float InDuration, float InKnockBackMultiplier);
		
	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	
	// OneShot, Periodic
	void PerformAreaDamage();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	//UFUNCTION() void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "PS")
	void AtPerformAreaDamage(const TArray<AActor*>& InAreaActors);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "PS")
	void AtOverlapBegin(const AActor* InActor);
	
private:
	void ReturnToPool();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlipbookComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "PS")
	float MoveSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OneShotDuration = 0.1f;
	
private:
    bool bInitialized = false;
	
	UPROPERTY()
	TArray<AActor*> AreaActors;	
	TArray<TWeakObjectPtr<AActor>> OverlapActors;	//처음 콜리전에 들어올 시 데미지. 풀에 반환할 때 비운다.
	
	FVector OriginalBoxExtent;
	FVector CurrentBoxExtent;

	ESummonBehavior SummonBehavior;
	ESummonDamageMode SummonDamageMode;

	FDamageResult FinalDamage;
	float KnockBackMultiplier = 0.f;
	
	FTimerHandle DurationTimerHandle;
    FTimerHandle HitboxDelayTimerHandle;
};
