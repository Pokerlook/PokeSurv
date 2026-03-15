// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/PSSkillComponent.h"

#include "PokeSurv/Character/PSCharacterEnemy.h"
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

FVector UPSSkillComponent::FindNearestEnemyLocation()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(1500.f); // 화면 크기에 맞게 조정

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	GetWorld()->OverlapMultiByObjectType(Overlaps, GetOwner()->GetActorLocation(),
		FQuat::Identity, ObjectParams, Sphere);

	AActor* Nearest = nullptr;
	float MinDist = FLT_MAX;
	FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (auto& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor || !Actor->IsA(APSCharacterEnemy::StaticClass())) continue;

		float Dist = FVector::DistSquared(OwnerLocation, Actor->GetActorLocation());
		if (Dist < MinDist)
		{
			MinDist = Dist;
			Nearest = Actor;
		}
	}

	return Nearest ? Nearest->GetActorLocation() : FVector::ZeroVector;
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
	
	EnemyScanTimer -= DeltaTime;
	if (EnemyScanTimer <= 0.f)
	{
		FVector EnemyLocation = FindNearestEnemyLocation();
		for (UPSSkillBase* Skill : Skills)
		{
			if (Skill) Skill->NearestEnemyLocation = EnemyLocation;
		}
		EnemyScanTimer = EnemyScanInterval;
	}
}

void UPSSkillComponent::AddSkill(TSubclassOf<UPSSkillBase> SkillClass)
{
	if (!SkillClass) return;
    
	UPSSkillBase* NewSkill = NewObject<UPSSkillBase>(this, SkillClass);
	NewSkill->InitSkill(GetOwner());
	Skills.Add(NewSkill);
	
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	PoolSubsystem->InitPool(NewSkill->SkillData->ProjectileClass, NewSkill->SkillData->ProjectileLimit);
}

