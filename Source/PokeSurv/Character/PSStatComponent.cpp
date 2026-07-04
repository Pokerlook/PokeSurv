// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSStatComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSStatComponent::Initialize(UPSPokemonData* NewData)
{
    if (NewData) BaseData = NewData;
	if (!BaseData)
	{
		UE_LOG(LogTemp, Error, TEXT("UPSStatComponent::Initialize: No PokemonData Data!"));
		return;
	}

	UPSPlayerStatComponent* PlayerStatComponent = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->FindComponentByClass<UPSPlayerStatComponent>();
	
	if(GetOwner()->ActorHasTag("Enemy"))
	{
		if(BaseData->bScaleHPByPlayerLevel)
		{
			MaxHP = BaseData->Health * (1 + PlayerStatComponent->Curse) * PlayerStatComponent->PlayerLevel;			
		}
		else
		{
			MaxHP = BaseData->Health * (1 + PlayerStatComponent->Curse);					
		}
		Defense = 0;
	}
	else if (GetOwner()->ActorHasTag("Player"))
	{
		MaxHP      = BaseData->Health * (1 + PlayerStatComponent->MaxHPBonus);	
		Defense    = PlayerStatComponent->Defense;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no Tag"), *GetOwner()->GetName());
	}
	
	CurrentHP  = MaxHP;
	Attack     = BaseData->Power;
	Speed      = BaseData->Speed;
	KnockbackMultiplier = BaseData->KnockbackMultiplier;
	
    OnStatsChanged.Broadcast(); // 캐릭터가 읽기, speed (AnimBP)
}

void UPSStatComponent::RecalculateFromPlayerStat(UPSPlayerStatComponent* PlayerStatComponent)
{
	if (!BaseData)
	{
		UE_LOG(LogTemp, Error, TEXT("UPSStatComponent::Initialize: No PokemonData Data!"));
		return;
	}
	if (!PlayerStatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UPSStatComponent::Initialize: No PlayerStatComponent!"));
		return;
	}
	
	float PrevMaxHP = MaxHP;
	float HPRatio = (PrevMaxHP > 0.f) ? (float)CurrentHP / (float)PrevMaxHP : 1.f;
	MaxHP     = BaseData->Health * (1 + PlayerStatComponent->MaxHPBonus);
    CurrentHP = MaxHP * HPRatio;
	Attack    = BaseData->Power;
	Defense   = PlayerStatComponent->Defense;
	Speed     = BaseData->Speed * (1 + PlayerStatComponent->SpeedBonus);
	
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
