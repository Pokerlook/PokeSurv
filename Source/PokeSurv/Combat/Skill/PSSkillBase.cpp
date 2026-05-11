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
	
	PlayerStatComp = Owner->FindComponentByClass<UPSPlayerStatComponent>();
	LevelUp();
}

void UPSSkillBase::LevelUp()
{
	if (SkillLevel >=8)
	{
		UE_LOG(LogTemp, Warning, TEXT("Over Level Up Tried"));
		return;
	}

	CurrentStats += SkillData->LevelData[SkillLevel];
	SkillLevel++;
}

void UPSSkillBase::DebugSkill()
{
	UE_LOG(LogTemp, Log, TEXT("=== [%s] SkillStats ==="), *GetName());
	UE_LOG(LogTemp, Log, TEXT("Cooldown: %f"), CurrentStats.Cooldown);
	UE_LOG(LogTemp, Log, TEXT("FireInterval: %f"), CurrentStats.FireInterval);
	UE_LOG(LogTemp, Log, TEXT("HitboxDelay: %f"), CurrentStats.HitboxDelay);
	UE_LOG(LogTemp, Log, TEXT("Damage: %f"), CurrentStats.Damage);
	UE_LOG(LogTemp, Log, TEXT("AreaMultiplier: %f"), CurrentStats.AreaMultiplier);
	UE_LOG(LogTemp, Log, TEXT("ProjectileSpeedMultiplier: %f"), CurrentStats.ProjectileSpeedMultiplier);
	UE_LOG(LogTemp, Log, TEXT("ProjectileCount: %d"), CurrentStats.ProjectileCount);
	UE_LOG(LogTemp, Log, TEXT("PierceCount: %d"), CurrentStats.PierceCount);
	UE_LOG(LogTemp, Log, TEXT("Duration: %f"), CurrentStats.Duration);
	UE_LOG(LogTemp, Log, TEXT("Knockback: %f"), CurrentStats.Knockback);
	UE_LOG(LogTemp, Log, TEXT("CritChance: %f"), CurrentStats.CritChance);
	UE_LOG(LogTemp, Log, TEXT("CritMultiplier: %f"), CurrentStats.CritMultiplier);
}

FDamageResult UPSSkillBase::CalculateDamage() const
{
    FDamageResult Result;
	
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return Result;
	}

	Result.bIsCritical = FMath::RandRange(0.f, 1.f) < CurrentStats.CritChance;
	float CritMult = Result.bIsCritical ? CurrentStats.CritMultiplier : 1.f;
    
	Result.FinalDamage = CurrentStats.Damage * CritMult * (1.0f + PlayerStatComp->DamageBonus);
	return Result;
}

float UPSSkillBase::CalculateAreaMultiplayer() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	const float Result = CurrentStats.AreaMultiplier + PlayerStatComp->AreaBonus;
	return Result;
}

float UPSSkillBase::CalculateProjectileSpeedMultiplayer() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	const float Result = CurrentStats.ProjectileSpeedMultiplier + PlayerStatComp->ProjectileSpeedBonus;
	return Result;
}

float UPSSkillBase::CalculateDuration() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	
	const float Result = CurrentStats.Duration + PlayerStatComp->DurationBonus;
	return Result;
}

float UPSSkillBase::CalculateCooldown() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	
	const float Result = CurrentStats.Cooldown * (1.0f + PlayerStatComp->CooldownDecrease);
	return Result;
}

int32 UPSSkillBase::CalculateProjectileCount() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	const int32 Result = CurrentStats.ProjectileCount * PlayerStatComp->ProjectileCount;
	return Result;
}
