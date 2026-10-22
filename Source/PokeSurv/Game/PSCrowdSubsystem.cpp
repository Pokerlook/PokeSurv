// Fill out your copyright notice in the Description page of Project Settings.


#include "PSCrowdSubsystem.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Component/PSCrowdMovementComponent.h"

void UPSCrowdSubsystem::Register(UPSCrowdMovementComponent* Agent)
{
	if (Agent)
	{
		Agents.AddUnique(Agent);
		CellSize = FMath::Max(CellSize, Agent->GetCrowdRadius() * 2.f);
	}
}

void UPSCrowdSubsystem::Unregister(UPSCrowdMovementComponent* Agent)
{
	Agents.RemoveSwap(Agent);
	// Snapshots는 값 사본이라 이번 프레임 안에서 제거돼도 안전. 다음 프레임 재구성 때 빠짐
}

bool UPSCrowdSubsystem::GetHeroInfo(FCrowdHeroInfo& Out)
{
	APawn* Hero = CachedHero.Get();
	if (!Hero)
	{
		Hero = UGameplayStatics::GetPlayerPawn(this, 0);
		if (!Hero)
		{
			return false;
		}
		CachedHero = Hero;

		if (const UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Hero->GetRootComponent()))
		{
			CachedHeroRadius = Capsule->GetScaledCapsuleRadius();
		}
	}

	Out.Pos = CrowdRules::To2D(Hero->GetActorLocation());
	Out.Vel = CrowdRules::To2D(Hero->GetVelocity());
	Out.Radius = CachedHeroRadius;
	return true;
}

void UPSCrowdSubsystem::RebuildIfNeeded()
{
	if (BuiltFrame == GFrameCounter)
	{
		return;
	}
	BuiltFrame = GFrameCounter;

	Snapshots.Reset(Agents.Num());
	Cells.Reset();

	for (const UPSCrowdMovementComponent* Agent : Agents)
	{
		if (!IsValid(Agent) || !Agent->UpdatedComponent)
		{
			continue;
		}

		FCrowdAgentSnapshot S;
		S.Owner = Agent;
		S.Pos = CrowdRules::To2D(Agent->UpdatedComponent->GetComponentLocation());
		S.Radius = Agent->GetCrowdRadius();
		S.Type = Agent->MoveType;

		const int32 Idx = Snapshots.Add(S);
		Cells.FindOrAdd(ToCell(S.Pos)).Add(Idx);
	}
}
