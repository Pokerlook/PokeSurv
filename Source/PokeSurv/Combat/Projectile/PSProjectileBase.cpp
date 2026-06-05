// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Projectile/PSProjectileBase.h"

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
	FlipbookComponent->SetCollisionProfileName(TEXT("NoCollision"));
	
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APSProjectileBase::OnOverlapBegin);
	CollisionSphere->OnComponentHit.AddDynamic(this, &APSProjectileBase::OnHit);

}

void APSProjectileBase::Initialize(AActor* InOwner, FDamageResult InDamage, float InAreaMultiplier, float InSpeedMultiplier, int32 InPierceCount,
	float InKnockBackMultiplier)
{
	Owner = InOwner;
	Tags.Empty();
	if (InOwner)
	{
		if (InOwner->ActorHasTag("Player")) Tags.Add("Player");
		else if (InOwner->ActorHasTag("Enemy")) Tags.Add("Enemy");
	}
	
	KnockBackMultiplier = InKnockBackMultiplier;
	FinalDamage = InDamage;
	PierceCount = InPierceCount;
	
	ProjectileMovement->InitialSpeed = DefaultSpeed * InSpeedMultiplier;
	ProjectileMovement->MaxSpeed = DefaultSpeed * InSpeedMultiplier;
	ProjectileMovement->Velocity = GetActorRotation().Vector() * DefaultSpeed * InSpeedMultiplier;//GetActorForwardVector() * DefaultSpeed * InSpeedMultiplier;
	CollisionSphere->SetSphereRadius(DefaultRadius * InAreaMultiplier);
	FlipbookComponent->SetRelativeScale3D(DefaultFlipBookScale*(InAreaMultiplier));

	bInitialized = true;
}

void APSProjectileBase::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool_Implementation();
}

void APSProjectileBase::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool_Implementation();
	
	ProjectileMovement->StopMovementImmediately();
	HitActors.Empty();
    bInitialized = false;
}

void APSProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultSpeed = ProjectileMovement->InitialSpeed;
	DefaultRadius = CollisionSphere->GetUnscaledSphereRadius();
    DefaultFlipBookScale = FlipbookComponent->GetRelativeScale3D(); 
}

void APSProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
	if (!bInitialized) return;
	if (!OtherActor || OtherActor == Owner)
	{
		return;
	}
	
	// 발사체 vs 발사체
	if (APSProjectileBase* OtherProjectile = Cast<APSProjectileBase>(OtherActor))
	{
		if (!UPSLibrary::IsHostileTo(this, OtherProjectile)) return;
        
		if (HitActors.Contains(OtherActor)) return;
		HitActors.Add(OtherActor);
        
		if (PierceCount != TNumericLimits<int32>::Max())
		{
			PierceCount--;
			if (PierceCount <= 0)
			{
				UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
				Pool->ReturnToPool(this);
			}
		}
		return;
	}
	
	if (!UPSLibrary::IsHostileTo(this, OtherActor))
	{
		return;
	}
	if (!UPSLibrary::IsAlive(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("  → return: not alive"));
		return;
	}
	
	// 중복 히트 방지
	if (HitActors.Contains(OtherActor))
	{
		return;
	}
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
	if (!bInitialized) return; 
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
}


