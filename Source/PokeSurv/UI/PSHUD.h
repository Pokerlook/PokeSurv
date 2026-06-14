// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PokeSurv/PSTypes.h"
#include "PSHUD.generated.h"

class UPSLevelUpMenuWidget;
/**
 * 
 */
UCLASS()
class POKESURV_API APSHUD : public AHUD
{
	GENERATED_BODY()

public:
	TObjectPtr<UPSLevelUpMenuWidget> GetOrCreateLevelUpMenu();
	void ShowLevelUpMenu(const TArray<FPSLevelUpOption>& Chosen) const;
	void HideLevelUpMenu() const;
	
protected:
	virtual void BeginPlay() override;

private:
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "PS|UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "PS|UI")
	TSubclassOf<UPSLevelUpMenuWidget> LevelUpMenuClass;
	
private:
	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidget;
	
    UPROPERTY()
    TObjectPtr<UPSLevelUpMenuWidget> LevelUpMenu;	
};
