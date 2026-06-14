// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PokeSurv/PSTypes.h"
#include "PSLevelUpMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCardSelected, FPSLevelUpOption, Selected);

/**
 * 
 */
UCLASS()
class POKESURV_API UPSLevelUpMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// C++ 전용. 외부(HUD)는 이걸 부름. BP가 오버라이드 불가.
	void SetupCards(const TArray<FPSLevelUpOption>& Candidates);
	
	// BP 카드 버튼 온클릭에서
	UFUNCTION(BlueprintCallable, Category = "PS")
	void NotifySelection(const FPSLevelUpOption& Selected);

protected:
	// 게임모드가 후보를 넘기면 BP가 카드 생성
	UFUNCTION(BlueprintImplementableEvent, Category = "PS")
	void OnSetupCards(const TArray<FPSLevelUpOption>& Candidates);


public:
	UPROPERTY(BlueprintAssignable, Category = "PS")
	FOnCardSelected OnCardSelected;
	
private:
	bool bSelectionLocked = false;
};
