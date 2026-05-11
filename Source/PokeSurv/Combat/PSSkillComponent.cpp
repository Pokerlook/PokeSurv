// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/PSSkillComponent.h"

#include "Item/PSItemBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "Projectile/PSProjectileBase.h"
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

	for(UPSSkillBase* Skill : Skills)
	{
		if(Skill->GetClass() == SkillClass)
		{
			Skill->LevelUp();
			return;
		}
	}
    
	UPSSkillBase* NewSkill = NewObject<UPSSkillBase>(this, SkillClass);
	
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	PoolSubsystem->InitPool(TSubclassOf<APSPoolableActor>(NewSkill->SkillData->ProjectileClass), NewSkill->SkillData->ProjectileLimit);
	
	NewSkill->InitSkill(GetOwner());
	Skills.Add(NewSkill);
}

void UPSSkillComponent::AddItem(TSubclassOf<UPSItemBase> ItemClass)
{
    // 이미 있으면 레벨업
    for (UPSItemBase* Item : Items)
    {
        if (Item->GetClass() == ItemClass)
        {
            Item->LevelUp();
            return;
        }
    }

    // 없으면 추가 
    UPSItemBase* NewItem = NewObject<UPSItemBase>(this, ItemClass);
    NewItem->InitItem(GetOwner());
    Items.Add(NewItem);
}

