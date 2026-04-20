// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/PSSkillComponent.h"

#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "Skill/PSSkillBase.h"

UPSSkillComponent::UPSSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPSSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultSkill)
	{
		AddSkill(DefaultSkill);
	}
}

void UPSSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UPSSkillBase* Skill : Skills)
	{
		if (Skill)
		{
			Skill->TickCooldown(DeltaTime);
		}
	}
}

void UPSSkillComponent::AddSkill(TSubclassOf<UPSSkillBase> SkillClass)
{
	if (!SkillClass) return;
    
	UPSSkillBase* NewSkill = NewObject<UPSSkillBase>(this, SkillClass);
	
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	PoolSubsystem->InitPool(NewSkill->SkillData->ProjectileClass, NewSkill->SkillData->ProjectileLimit);
	
	NewSkill->InitSkill(GetOwner());
	Skills.Add(NewSkill);
}

