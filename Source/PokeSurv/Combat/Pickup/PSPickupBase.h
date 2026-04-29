// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Actor/PSPoolableActor.h"
#include "PSPickupBase.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API APSPickupBase : public APSPoolableActor
{
	GENERATED_BODY()
	
public:
	APSPickupBase();
	virtual void OnSpawnFromPool_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnPickupCollected(AActor* Target);	// 스탯을 변경하거나 이벤트를 발생시킬 것

	// OnPickupCollected에 추가할 것
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void ReturnToPool();

	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void SetValue(float InValue);
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<class USphereComponent> CollisionSphere;
	
	UPROPERTY(BlueprintReadWrite, Category = "Pickup")
	float Value = 0.f;
};
