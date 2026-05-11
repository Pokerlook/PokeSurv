// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Item/PSItemBase.h"

#include "PokeSurv/Data/PSItemData.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSItemBase::InitItem(AActor* InOwner)
{
	if (ItemLevel != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PSItemBase::InitItem - ItemLevel is not 0"));
		return;
	}

	Owner = InOwner;
	PlayerStatComp = InOwner->FindComponentByClass<UPSPlayerStatComponent>();
	
	LevelUp(); // 0 → 1
}

void UPSItemBase::LevelUp()
{
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] LevelUp - ItemData is null"), *GetName());
		return;
	}
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] LevelUp - PlayerStatComp is null"), *GetName());
		return;
	}
	if (ItemLevel >= ItemData->MaxLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] LevelUp - Already max level %d"), *GetName(), ItemLevel);
		return;
	}

	for (const FItemStatEntry& Entry : ItemData->StatPerLevel)
	{
		PlayerStatComp->AddStat(Entry.StatType, Entry.Value);
	}
	ItemLevel++;
}
