// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PSCharacterPlayer.generated.h"

class UPSSkillComponent;
class APSPlayerController;
class UPSPlayerStatComponent;
class UPSHealthWidgetComponent;

/**
 * 
 */
UCLASS()
class POKESURV_API APSCharacterPlayer : public APSCharacterBase
{
	GENERATED_BODY()
public:
	APSCharacterPlayer();
	
	virtual void PS_TakeDamage_Implementation(const FDamageResult  Damage) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void UpdateMoveVector(float DeltaSeconds) override;

private:
	UPROPERTY()
	TObjectPtr<APSPlayerController> PSPlayerController;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSPlayerStatComponent> PlayerStatComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSSkillComponent> SkillComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPSHealthWidgetComponent> HPBarComponent;
};
