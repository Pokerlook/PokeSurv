// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSStatComponent.h"

#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSStatComponent::Initialize(UPSPokemonData* NewData)
{
    if (NewData) BaseData = NewData;
	if (!BaseData)
	{
		UE_LOG(LogTemp, Error, TEXT("UPSStatComponent::Initialize: No PokemonData Data!"));
		return;
	}

	MaxHP      = BaseData->Health;
	CurrentHP  = MaxHP;
	Attack     = BaseData->Power;
	Defense    = 0;               // 적 기본 방어력 없음 (뱀서 방식)
	Speed      = BaseData->Speed;
	KnockbackMultiplier = BaseData->KnockbackMultiplier;
	
    OnStatsChanged.Broadcast(); // 캐릭터가 읽기
}

void UPSStatComponent::RecalculateFromPlayerStat(UPSPlayerStatComponent* PlayerStatComponent)
{
	if (!BaseData)
	{
		UE_LOG(LogTemp, Error, TEXT("UPSStatComponent::Initialize: No PokemonData Data!"));
		return;
	}

	MaxHP      = BaseData->Health * (1 + PlayerStatComponent->MaxHPBonus) ;
	CurrentHP  = MaxHP;
	Attack     = BaseData->Power;
	Defense    = PlayerStatComponent->Defense;
	Speed      = BaseData->Speed * (1 + PlayerStatComponent->SpeedBonus) ;

	
	MaxHP      = BaseData->Health * (1 + PlayerStatComponent->Curse) ;
	CurrentHP  = MaxHP;
	Attack     = BaseData->Power;
	Defense    = 0;               // 적 기본 방어력 없음 (뱀서 방식)
	Speed      = BaseData->Speed;

	
	KnockbackMultiplier = BaseData->KnockbackMultiplier;
	
	OnStatsChanged.Broadcast();
}

void UPSStatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (BaseData)
	{
		Initialize(BaseData);
	}
}
