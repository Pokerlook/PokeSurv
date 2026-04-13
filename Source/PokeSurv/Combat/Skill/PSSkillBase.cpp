// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Skill/PSSkillBase.h"

#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSSkillBase::TickCooldown(float DeltaTime)
{
	if (!SkillData) return;
    
	CooldownTimer -= DeltaTime;
	if (CooldownTimer <= 0.f)
	{
		Activate();
		CooldownTimer = CurrentStats.Cooldown;
	}
}

void UPSSkillBase::InitSkill(AActor* InOwner)
{
	Owner = InOwner;
	CooldownTimer = 0.f; // 바로 발동되게
	CurrentStats = SkillData->LevelData[0];
}

float UPSSkillBase::CalculateDamage() const
{
	const UPSPlayerStatComponent* PlayerStatComp = Owner->FindComponentByClass<UPSPlayerStatComponent>();
	if (!PlayerStatComp) return 0.f;

	return (PlayerStatComp->DamageBonus+1) * CurrentStats.Damage;
}
