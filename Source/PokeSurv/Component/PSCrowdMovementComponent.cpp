// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Component/PSCrowdMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "PokeSurv/Game/PSCrowdSubsystem.h"

using namespace CrowdRules;

UPSCrowdMovementComponent::UPSCrowdMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UPSCrowdMovementComponent::AddKnockback(const FVector& WorldImpulse)
{
	KnockbackVel += To2D(WorldImpulse);
}

void UPSCrowdMovementComponent::ResetMotion()
{
	Velocity = FVector::ZeroVector;
	KnockbackVel = FVector2f::ZeroVector;
}

void UPSCrowdMovementComponent::OnSpawnFromPool()
{
	if (UPSCrowdSubsystem* Registry = GetWorld()->GetSubsystem<UPSCrowdSubsystem>())
	{
		Registry->Register(this);
	}
}

void UPSCrowdMovementComponent::OnReturnFromPool()
{
	if (UWorld* World = GetWorld())
	{
		if (UPSCrowdSubsystem* Registry = World->GetSubsystem<UPSCrowdSubsystem>())
		{
			Registry->Unregister(this);
		}
	}
}

void UPSCrowdMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(UpdatedComponent))
	{
		Radius = Capsule->GetScaledCapsuleRadius();
	}

}

void UPSCrowdMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const FVector Input = ConsumeInputVector();

	if (ShouldSkipUpdate(DeltaTime) || !UpdatedComponent)
	{
		return;
	}

	UPSCrowdSubsystem* Registry = GetWorld()->GetSubsystem<UPSCrowdSubsystem>();
	if (!Registry)
	{
		return;
	}

	const FVector StartLoc = UpdatedComponent->GetComponentLocation();
	FVector2f Pos = To2D(StartLoc);
	FVector2f Vel = To2D(Velocity);

	FCrowdHeroInfo Hero;
	const bool bHasHero = Registry->GetHeroInfo(Hero);

	// 1) 가고 싶은 방향
	FVector2f DesiredDir = FVector2f::ZeroVector;
	if (MoveType == ECrowdMoveType::Flow)
	{
		const FVector2f Flow = To2D(FlowDirection).GetSafeNormal();
		DesiredDir = bHasHero
			? FlowWithAvoid(Flow, Pos, Hero.Pos, Tuning.AvoidLookAhead,
				Radius + Hero.Radius + Tuning.AvoidMargin, Tuning.AvoidStrength)
			: Flow;
	}
	else if (bChaseHero && bHasHero)
	{
		DesiredDir = (Hero.Pos - Pos).GetSafeNormal();
	}
	else
	{
		DesiredDir = To2D(Input).GetSafeNormal();
	}

	// 2) 가속
	Vel = MoveTowards(Vel, DesiredDir * MaxSpeed, Acceleration * DeltaTime);

	// 3) Chase: A를 못 밀고 막힘
	bBlockedByHero = false;
	if (MoveType == ECrowdMoveType::Chase && bHasHero)
	{
		Vel = BlockTowardHero(Vel, Pos, Radius, Hero.Pos, Hero.Radius, Tuning.HeroContactPadding, bBlockedByHero);
	}
	BlockedTime = bBlockedByHero ? BlockedTime + DeltaTime : 0.f;

	// 4) 이동 (의도 + 넉백)
	Pos += (Vel + KnockbackVel) * DeltaTime;

	KnockbackVel *= FMath::Pow(KnockbackDamping, DeltaTime);
	if (KnockbackVel.SizeSquared() < 1.f)
	{
		KnockbackVel = FVector2f::ZeroVector;
	}

	// 5) 이웃 겹침 해소
	FVector2f NeighborCorr = FVector2f::ZeroVector;
	Registry->ForEachNeighbor(Pos, this, [&](const FCrowdAgentSnapshot& Other)
	{
		NeighborCorr += PairCorrection(Pos, Radius, MoveType, Other.Pos, Other.Radius, Other.Type);
	});
	Pos += ClampSize(NeighborCorr * Tuning.Stiffness, Tuning.MaxCorrectionPerFrame);

	// 6) A와 겹침: 100% 내가 비킴
	if (bHasHero)
	{
		const FVector2f HeroCorr = HeroCorrection(Pos, Radius, Hero.Pos, Hero.Radius, Hero.Vel, Tuning.HeroLateralBias);
		Pos += ClampSize(HeroCorr, Tuning.HeroMaxCorrectionPerFrame);
	}

	// 7) 적용
	const FVector NewLoc = To3D(Pos, StartLoc);
	UpdatedComponent->SetWorldLocation(NewLoc, false, nullptr, ETeleportType::None);

	Velocity = To3D(Vel, FVector::ZeroVector); // 의도 속도만
	UpdateComponentVelocity();

	UpdateFacing(Vel);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		const FColor Color = bBlockedByHero ? FColor::Red : (MoveType == ECrowdMoveType::Chase ? FColor::Yellow : FColor::Cyan);
		const FVector AxisY = FVector(0, 1, 0);
		DrawDebugCircle(GetWorld(), NewLoc, Radius, 16, Color, false, -1.f, 0, 1.5f, FVector(1, 0, 0), AxisY, false);
	}
#endif
}

void UPSCrowdMovementComponent::UpdateFacing(const FVector2f& Vel)
{
	if (Vel.SizeSquared() > FacingChangeThreshold * FacingChangeThreshold)
	{
		const FVector2f N = Vel.GetSafeNormal();
		FacingDirection = FVector2D(N.X, N.Y);
	}
	
}
