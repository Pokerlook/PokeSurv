// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Skill/PSSkillBase.h"

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
	CooldownTimer = 0.f; // 게임 시작하자마자 바로 발동되게
	CurrentStats = SkillData->LevelData[0];
}