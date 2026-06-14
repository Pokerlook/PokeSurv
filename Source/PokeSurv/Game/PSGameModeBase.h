// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PokeSurv/PSTypes.h"
#include "PSGameModeBase.generated.h"

class APSPickupBase;
class UPSSkillBase;
class UPSItemBase;
class UPSPlayerStatComponent;
class UPSSkillComponent;
class APSHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillCountChanged, int32, KillCount);

/**
 * 
 */
UCLASS()
class POKESURV_API APSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SpawnExpGem(FVector Location, float XPValue);
	
	void AddKillCount();
	
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPlayerLevelUp(int32 NewLevel);

	UFUNCTION()
	void HandleCardSelected(FPSLevelUpOption SelectedEntry);
	
	void LevelUpSequence();
	
	TArray<FPSLevelUpOption> BuildCandidatePool();
	TArray<FPSLevelUpOption> SelectCandidates(TArray<FPSLevelUpOption>& Pool);
	int32 PickOwnedIndex(TArray<FPSLevelUpOption>& Pool);
	int32 PickRandomIndex(TArray<FPSLevelUpOption>& Pool);
	
	float GetTotalLuck() const;
	bool RollFourthOption() const;
	bool RollOfferOwned() const; // 1회 체크 (호출부에서 2번 부름)
	
	int32 GetEntryWeight(const FPSLevelUpOption& Entry) const;
	
	APSHUD* GetPSHUD() const;
	
public:
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnKillCountChanged OnKillCountChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "PS")
	TSubclassOf<APSPickupBase> ExpGemClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "PS")
	TSet<TSubclassOf<UPSSkillBase>> AllSkillClasses;

	UPROPERTY(EditDefaultsOnly, Category = "PS")
	TSet<TSubclassOf<UPSItemBase>> AllItemClasses;
	
private:
	TObjectPtr<UPSPlayerStatComponent> PlayerStatComponent;
	TObjectPtr<UPSSkillComponent> SkillComponent;
	
	UPROPERTY()
	float PendingXP = 0.f;
	
	UPROPERTY()
	int32 KillCount = 0;
	
	int32 PendingLevelUps = 0;
	bool bMenuBound = false;
    bool bMenuOpen = false;
};
