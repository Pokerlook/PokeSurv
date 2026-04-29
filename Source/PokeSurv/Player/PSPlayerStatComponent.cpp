// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSPlayerStatComponent::AddStat(EPlayerStatType StatType, float Value)
{
	switch (StatType)
	{
		// Character
	case EPlayerStatType::MaxHPBonus:           MaxHPBonus          += (int32)Value; break;
	case EPlayerStatType::Recovery:             Recovery            += Value;        break;
	case EPlayerStatType::Defense:              Defense             += (int32)Value; break;
	case EPlayerStatType::SpeedBonus:           SpeedBonus          += Value;        break;

		// Skill
	case EPlayerStatType::DamageBonus:          DamageBonus         += Value;        break;
	case EPlayerStatType::ProjectileSpeedBonus: ProjectileSpeedBonus += Value;       break;
	case EPlayerStatType::DurationBonus:        DurationBonus       += Value;        break;
	case EPlayerStatType::AreaBonus:            AreaBonus           += Value;        break;
	case EPlayerStatType::CooldownDecrease:     CooldownDecrease    += Value;        break;
	case EPlayerStatType::ProjectileCount:      ProjectileCount     += (int32)Value; break;

		// Meta
	case EPlayerStatType::Revive:               Revive              += (int32)Value; break;
	case EPlayerStatType::MagnetBonus:          MagnetBonus         += Value;        break;
	case EPlayerStatType::Luck:                 Luck                += Value;        break;
	case EPlayerStatType::Growth:               Growth              += Value;        break;
	case EPlayerStatType::Greed:                Greed               += Value;        break;
	case EPlayerStatType::Curse:                Curse               += Value;        break;

		// Shop
	case EPlayerStatType::Reroll:               Reroll              += (int32)Value; break;
	case EPlayerStatType::Skip:                 Skip                += (int32)Value; break;
	case EPlayerStatType::Banish:               Banish              += (int32)Value; break;
	}

	OnPlayerStatChanged.Broadcast();
}

float UPSPlayerStatComponent::GetStat(EPlayerStatType StatType) const
{
	switch (StatType)
	{
	case EPlayerStatType::MaxHPBonus:           return (float)MaxHPBonus;
	case EPlayerStatType::Recovery:             return Recovery;
	case EPlayerStatType::Defense:              return (float)Defense;
	case EPlayerStatType::SpeedBonus:           return SpeedBonus;
	case EPlayerStatType::DamageBonus:          return DamageBonus;
	case EPlayerStatType::ProjectileSpeedBonus: return ProjectileSpeedBonus;
	case EPlayerStatType::DurationBonus:        return DurationBonus;
	case EPlayerStatType::AreaBonus:            return AreaBonus;
	case EPlayerStatType::CooldownDecrease:     return CooldownDecrease;
	case EPlayerStatType::ProjectileCount:      return (float)ProjectileCount;
	case EPlayerStatType::Revive:               return (float)Revive;
	case EPlayerStatType::MagnetBonus:          return MagnetBonus;
	case EPlayerStatType::Luck:                 return Luck;
	case EPlayerStatType::Growth:               return Growth;
	case EPlayerStatType::Greed:                return Greed;
	case EPlayerStatType::Curse:                return Curse;
	case EPlayerStatType::Reroll:               return (float)Reroll;
	case EPlayerStatType::Skip:                 return (float)Skip;
	case EPlayerStatType::Banish:               return (float)Banish;
	default:                                    return 0.f;
	}
}

void UPSPlayerStatComponent::AddExp(float Value)
{
	Exp += Value;
	if(Exp >= GetRequiredExp())
	{
		//LevelUp //Value -= GetRequiredExp();
		UE_LOG(LogTemp, Warning, TEXT("LevelUp"));
	}
}

int32 UPSPlayerStatComponent::GetRequiredExp() const
{
	if (PlayerLevel == 1)
		return 5;

	if (PlayerLevel < 20)
		return 5 + (PlayerLevel - 1) * 10;

	if (PlayerLevel == 20)
		return 5 + 19 * 10 + 600; // 795

	if (PlayerLevel < 40)
		return 795 + (PlayerLevel - 20) * 13;

	if (PlayerLevel == 40)
		return 795 + 20 * 13 + 2400; // 3455

	// 41+
	return 3455 + (PlayerLevel - 40) * 16;
}
