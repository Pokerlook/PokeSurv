// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Pawn/PSEnemyBossPawn.h"

#include "PokeSurv/Combat/Pickup/PSPickupBase.h"

void APSEnemyBossPawn::SetTreasure(const TSubclassOf<APSPickupBase>& InTreasureClass)
{
	TreasureChestClass = InTreasureClass;
}

void APSEnemyBossPawn::HandleDrops()
{
	Super::HandleDrops();  // 경험치 보석

	if (TreasureChestClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APSPickupBase* Treasure = GetWorld()->SpawnActor<APSPickupBase>(
			TreasureChestClass,
			GetActorLocation(),
			FRotator::ZeroRotator,
			Params);	// 부모인 APSCharacterEnemy가 IPSPoolable 이긴 한데, 보물상자는 몇 개 없으니. 풀링을 하지 않는다.
		
	}
}
