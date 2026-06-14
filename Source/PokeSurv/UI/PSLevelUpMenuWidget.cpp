// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/UI/PSLevelUpMenuWidget.h"


void UPSLevelUpMenuWidget::SetupCards(const TArray<FPSLevelUpOption>& Candidates)
{
    bSelectionLocked = false;
    OnSetupCards(Candidates);
}

void UPSLevelUpMenuWidget::NotifySelection(const FPSLevelUpOption& Selected)
{
    if (bSelectionLocked) return;   // 두 번째 클릭 차단
    bSelectionLocked = true;
    OnCardSelected.Broadcast(Selected);
}
