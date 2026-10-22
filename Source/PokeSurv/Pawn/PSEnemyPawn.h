// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PokeSurv/Interface/PSDamageable.h"
#include "PokeSurv/Interface/PSPoolable.h"
#include "PSEnemyPawn.generated.h"

class UPSCrowdMovementComponent;
class UCapsuleComponent;
class UPaperFlipbookComponent;
class UPSStatComponent;

UCLASS()
class POKESURV_API APSEnemyPawn : public APawn, public IPSDamageable, public IPSPoolable
{
	GENERATED_BODY()

public:
	APSEnemyPawn();
	
	virtual void PS_TakeDamage_Implementation(const FDamageResult Damage) override;
	virtual void TakeKnockback_Implementation(float KnockBackMultiplier, FVector HitLocation) override;

	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
	virtual bool ShouldCull_Implementation() override;

	TObjectPtr<UCapsuleComponent> GetCapsuleComponent(){return Capsule;}
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "PS")
	void OnDamage();
	UFUNCTION(BlueprintNativeEvent, Category = "PS")
	void OnDeath();

	virtual void HandleDrops();
	
	UFUNCTION(BlueprintCallable)
	void RequestReturnToPool();		// BP에서 Death때 쓸 것

	// 접촉 데미지 (임시 — 나중에 PSCrowdSubsystem으로 이사)
	void ContactDamageTick();
	
private:
	UFUNCTION()
	void OnStatsChanged();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UPaperFlipbookComponent> Sprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UPSCrowdMovementComponent> CrowdMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSStatComponent> StatComponent;

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnStatsInitialized OnStatsInitializedDelegate;
	
private:
	FTimerHandle ContactDamageTimer;
	float ContactDamageInterval = 0.5f;
};
