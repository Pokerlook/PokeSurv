// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Actor/PSProjectileBase.h"

#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Util/PSLibrary.h"

// Sets default values
APSProjectileBase::APSProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    RootComponent = CollisionSphere;

    FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
    FlipbookComponent->SetupAttachment(RootComponent);
	
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APSProjectileBase::OnOverlapBegin);
	CollisionSphere->OnComponentHit.AddDynamic(this, &APSProjectileBase::OnHit);

}

void APSProjectileBase::Initialize(AActor* InOwner, float InDamage, float InAreaMultiplier, float InSpeedMultiplier, int32 InPierceCount,
	float InKnockBackMultiplier)
{
	Owner = InOwner;
	KnockBackMultiplier = InKnockBackMultiplier;
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
	if (!OtherActor || OtherActor == Owner) return;
	if (!OtherActor->IsA<APSCharacterBase>()) return;
	
	// 중복 히트 방지
	if (HitActors.Contains(OtherActor)) return;
	HitActors.Add(OtherActor);

	// 데미지 처리
    UPSLibrary::ApplyDamage(OtherActor, FinalDamage, KnockBackMultiplier, this);

	// 관통 처리
	if (PierceCount != TNumericLimits<int32>::Max())
	{
		PierceCount--;
		if (PierceCount <= 0)  // 이미 0이면 이번 히트 후 반환
		{
			UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
			Pool->ReturnToPool(this);
		}
	}
}

void APSProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
}


