// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/PSStatComponent.h"

void UPSStatComponent::Initialize(UPokemonData* NewData)
{
    if (NewData) BaseData = NewData;
	if (!BaseData)
	{
		return;
	}

	MaxHP      = BaseData->Health;
	CurrentHP  = MaxHP;
	Attack     = BaseData->Power;
	Defense    = 0;               // 적 기본 방어력 없음 (뱀서 방식)
	Speed      = BaseData->Speed;
	KnockbakMultiplier = BaseData->KnockbackMultiplier.X;
	
    OnStatsChanged.Broadcast(); // 캐릭터가 
}

void UPSStatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (BaseData)
	{
		Initialize(BaseData);
	}
}