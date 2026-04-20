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

FDamageResult UPSSkillBase::CalculateDamage() const
{
    FDamageResult Result;
	
	const UPSPlayerStatComponent* PlayerStatComp = Owner->FindComponentByClass<UPSPlayerStatComponent>();
	if (!PlayerStatComp) return Result;

	Result.bIsCritical = FMath::RandRange(0.f, 1.f) < CurrentStats.CritChance;
	float CritMult = Result.bIsCritical ? CurrentStats.CritMultiplier : 1.f;
    
	Result.FinalDamage = (PlayerStatComp->DamageBonus + 1) * CurrentStats.Damage * CritMult;
	return Result;
}
