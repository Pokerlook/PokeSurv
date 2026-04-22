// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/UI/PSHealthWidgetComponent.h"

void UPSHealthWidgetComponent::UpdateHP(float CurrentHP, float MaxHP) const
{
    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}
