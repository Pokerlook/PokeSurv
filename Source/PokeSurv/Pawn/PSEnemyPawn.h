// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PokeSurv/Interface/PSDamageable.h"
#include "PokeSurv/Interface/PSPoolable.h"
#include "Sequencer/IPaperZDSequencerSource.h"
#include "PSEnemyPawn.generated.h"

class UPSCrowdMovementComponent;
class UCapsuleComponent;
class UPaperFlipbookComponent;
class UPSStatComponent;
class UPaperZDAnimationComponent;

UCLASS()
class POKESURV_API APSEnemyPawn : public APawn, public IPSDamageable, public IPSPoolable, public IPaperZDSequencerSource
{
	GENERATED_BODY()

public:
	APSEnemyPawn();
	
	virtual void PS_TakeDamage_Implementation(const FDamageResult Damage) override;
	virtual void TakeKnockback_Implementation(float KnockBackMultiplier, FVector HitLocation) override;

	virtual void OnSpawnFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
	virtual bool ShouldCull_Implementation() override;

	UCapsuleComponent* GetCapsuleComponent() const {return Capsule;}
	
	//~ Begin IPaperZDSequencerSource Interface
	virtual TSubclassOf<UPaperZDAnimInstance> GetSequencerAnimInstanceClass() const override;
	virtual UPaperZDAnimInstance* GetSequencerAnimInstance() override;
	//~ End IPaperZDSequencerSource Interface
	
	UFUNCTION(BlueprintPure)
	FVector2D GetFacingDirection() const;

	UFUNCTION(BlueprintPure)
	float GetMoveSpeed() const { return GetVelocity().Size2D(); }
	
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
	
	void ActivateEnemy();
	void DeactivateEnemy();
	
private:
	UFUNCTION()
	void OnStatsChanged();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<UPaperFlipbookComponent> Sprite;
	
	/* PaperZD 애니메이션 컴포넌트 (AnimBP는 여기 Anim Instance Class에 지정) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPaperZDAnimationComponent> AnimationComponent;

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
