// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Actor/PSProjectileBase.h"

#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"

// Sets default values
APSProjectileBase::APSProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APSProjectileBase::OnOverlapBegin);
    RootComponent = CollisionSphere;

    FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
    FlipbookComponent->SetupAttachment(RootComponent);
	
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void APSProjectileBase::Initialize(float InDamage, float InAreaMultiplier, float InSpeedMultiplier, int32 InPierceCount,
	float InKnockBack)
{
	KnockBack = InKnockBack;
	FinalDamage = InDamage;
	PierceCount = InPierceCount;
	
	ProjectileMovement->InitialSpeed = DefaultSpeed * InSpeedMultiplier;
	ProjectileMovement->MaxSpeed = DefaultSpeed * InSpeedMultiplier;
	CollisionSphere->SetSphereRadius(DefaultRadius * InAreaMultiplier);
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
}

void APSProjectileBase::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool_Implementation();
	
	GetWorldTimerManager().SetTimer(LifeSpanTimer, [this]()
	{
		UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
		Pool->ReturnToPool(this);
	}, ProjectileLifeSpan, false);
	
}

void APSProjectileBase::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool_Implementation();
	
	GetWorldTimerManager().ClearTimer(LifeSpanTimer);
	ProjectileMovement->StopMovementImmediately();
	HitActors.Empty();
}

void APSProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultSpeed = ProjectileMovement->InitialSpeed;
	DefaultRadius = CollisionSphere->GetUnscaledSphereRadius();
}

void APSProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	IsValid 체크
	HitActors.Contains(Actor) — 있으면 무시
	없으면 HitActors.Add(Actor) 후 데미지

	피어스 카운트 처리...
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
	*/
}


