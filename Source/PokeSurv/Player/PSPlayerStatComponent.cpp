// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSPlayerStatComponent::BroadcastInitialValues()
{
	OnExpChanged.Broadcast(Exp, GetRequiredExp());
	OnGoldChanged.Broadcast(Gold);
	OnPlayerStatChanged.Broadcast();
}

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
	
	while (Exp >= GetRequiredExp())
	{
		Exp -= GetRequiredExp();
		LevelUp();
	}
	
    OnExpChanged.Broadcast(Exp, GetRequiredExp());
}

void UPSPlayerStatComponent::AddGold(float Value)
{
	Gold +=Value;
	OnGoldChanged.Broadcast(Gold);
}

void UPSPlayerStatComponent::DebugPlayerStat()
{
	UE_LOG(LogTemp, Log, TEXT("=== [%s] PlayerStat ==="), *GetOwner()->GetName());

	// Player
	UE_LOG(LogTemp, Log, TEXT("[Player]"));
	UE_LOG(LogTemp, Log, TEXT("  PlayerLevel: %d"), PlayerLevel);
	UE_LOG(LogTemp, Log, TEXT("  Exp: %.2f"), Exp);
	UE_LOG(LogTemp, Log, TEXT("  Gold: %d"), Gold);

	// Character
	UE_LOG(LogTemp, Log, TEXT("[Character]"));
	UE_LOG(LogTemp, Log, TEXT("  MaxHPBonus: %d"), MaxHPBonus);
	UE_LOG(LogTemp, Log, TEXT("  Recovery: %.2f"), Recovery);
	UE_LOG(LogTemp, Log, TEXT("  Defense: %d"), Defense);
	UE_LOG(LogTemp, Log, TEXT("  SpeedBonus: %.2f"), SpeedBonus);

	// Skill
	UE_LOG(LogTemp, Log, TEXT("[Skill]"));
	UE_LOG(LogTemp, Log, TEXT("  DamageBonus: %.2f"), DamageBonus);
	UE_LOG(LogTemp, Log, TEXT("  ProjectileSpeedBonus: %.2f"), ProjectileSpeedBonus);
	UE_LOG(LogTemp, Log, TEXT("  DurationBonus: %.2f"), DurationBonus);
	UE_LOG(LogTemp, Log, TEXT("  AreaBonus: %.2f"), AreaBonus);
	UE_LOG(LogTemp, Log, TEXT("  CooldownDecrease: %.2f"), CooldownDecrease);
	UE_LOG(LogTemp, Log, TEXT("  ProjectileCount: %d"), ProjectileCount);

	// Meta
	UE_LOG(LogTemp, Log, TEXT("[Meta]"));
	UE_LOG(LogTemp, Log, TEXT("  Revive: %d"), Revive);
	UE_LOG(LogTemp, Log, TEXT("  MagnetBonus: %.2f"), MagnetBonus);
	UE_LOG(LogTemp, Log, TEXT("  Luck: %.2f"), Luck);
	UE_LOG(LogTemp, Log, TEXT("  Growth: %.2f"), Growth);
	UE_LOG(LogTemp, Log, TEXT("  Greed: %.2f"), Greed);
	UE_LOG(LogTemp, Log, TEXT("  Curse: %.2f"), Curse);

	// Shop
	UE_LOG(LogTemp, Log, TEXT("[Shop]"));
	UE_LOG(LogTemp, Log, TEXT("  Reroll: %d"), Reroll);
	UE_LOG(LogTemp, Log, TEXT("  Skip: %d"), Skip);
	UE_LOG(LogTemp, Log, TEXT("  Banish: %d"), Banish);
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

void UPSPlayerStatComponent::LevelUp()
{
    PlayerLevel++;
    OnLevelUp.Broadcast(PlayerLevel);
}
