// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Skill/PSSkillBase.h"

#include "PokeSurv/Player/PSPlayerController.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSSkillBase::TickCooldown(float DeltaTime)
{
	if (!SkillData)
	{
		return;
	}
	
	// 발사 중에는 쿨다운 진행 중지
	if (RemainingFireCount > 0)
	{
		return;
	}
	
	CooldownTimer -= DeltaTime;
	if (CooldownTimer <= 0.f)
	{
		Activate();
		CooldownTimer = CalculateCooldown();;
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

void UPSSkillBase::Activate()
{
	StartFireSequence();
}

void UPSSkillBase::StartFireSequence()
{
	if (RemainingFireCount > 0)
	{
		return;
	}

	RemainingFireCount = CalculateProjectileCount();
	if (RemainingFireCount <= 0)
	{
		return;
	}

	// 첫 발은 즉시 발사
	HandleFireTick();
}

void UPSSkillBase::HandleFireTick()
{
	if (RemainingFireCount <= 0)
	{
		return;
	}

	FireOnce();
	RemainingFireCount--;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (RemainingFireCount > 0)
	{
		const float Interval = CurrentStats.FireInterval;

		if (Interval <= KINDA_SMALL_NUMBER)
		{
			// 간격 0이면 한 프레임 안에 다 발사
			while (RemainingFireCount > 0)
			{
				FireOnce();
				RemainingFireCount--;
			}
		}
		else
		{
			World->GetTimerManager().SetTimer(
				FireIntervalTimerHandle,
				this,
				&UPSSkillBase::HandleFireTick,
				Interval,
				false
			);
		}
	}
	else
	{
		World->GetTimerManager().ClearTimer(FireIntervalTimerHandle);
	}
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

float UPSSkillBase::CalculateAreaMultiplier() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	const float Result = CurrentStats.AreaMultiplier + PlayerStatComp->AreaBonus;
	return Result;
}

float UPSSkillBase::CalculateProjectileSpeedMultiplier() const
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
	
	const float Result = CurrentStats.Duration *(1.0f + PlayerStatComp->DurationBonus);
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
	const int32 Result = CurrentStats.ProjectileCount + PlayerStatComp->ProjectileCount;
	return Result;
}

int32 UPSSkillBase::CalculateProjectilePierceCount() const
{
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerStatComp is null"));
		return 0;
	}
	const int32 Result = CurrentStats.PierceCount; // + PlayerStatComp->ProjectileCount;
	return Result;
}

float UPSSkillBase::GetKnockBack() const
{
	return CurrentStats.Knockback;
}

float UPSSkillBase::GetFireInterval() const
{
	return CurrentStats.FireInterval;
}

float UPSSkillBase::GetHitBoxDelay() const
{
	return CurrentStats.HitboxDelay;
}

