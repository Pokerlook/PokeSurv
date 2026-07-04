// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h" 
#include "PokeSurv/Interface/PSDamageable.h"
#include "PSCharacterBase.generated.h"

class UPSStatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsInitialized);

UCLASS()
class POKESURV_API APSCharacterBase : public APaperZDCharacter, public IPSDamageable
{
	GENERATED_BODY()
	
public:
	APSCharacterBase();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PS_TakeDamage_Implementation(const FDamageResult  Damage) override;
	virtual void TakeKnockback_Implementation(float KnockBackMultiplier, FVector HitLocation) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void UpdateMoveVector(float DeltaSeconds) PURE_VIRTUAL(APSCharacterBase::UpdateMoveVector,);
	
	UFUNCTION(BlueprintNativeEvent, Category = "PS")
	void OnDamage();
	
	UFUNCTION(BlueprintNativeEvent, Category = "PS")
	void OnDeath();
	
	UFUNCTION(BlueprintNativeEvent, Category = "PS")
	void OnRevive();

private:
	UFUNCTION()
	void OnStatsChanged();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	FVector2D MoveVector2D = FVector2D(0, -1);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float MoveSpeed;

	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnStatsInitialized OnStatsInitializedDelegate; // AnimBP에서 Sprite변경
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSStatComponent> StatComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSound;  // 피격 사운드
};
