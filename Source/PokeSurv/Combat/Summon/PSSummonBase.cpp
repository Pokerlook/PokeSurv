// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Summon/PSSummonBase.h"

#include "PaperFlipbookComponent.h"
#include "Components/BoxComponent.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PokeSurv/Combat/Projectile/PSProjectileBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Util/PSLibrary.h"

APSSummonBase::APSSummonBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
    
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootComponent);
    
	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	FlipbookComponent->SetupAttachment(CollisionBox);
	FlipbookComponent->SetLooping(false);
	FlipbookComponent->SetCollisionProfileName(TEXT("NoCollision"));
    
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APSSummonBase::OnOverlapBegin);
	//CollisionBox->OnComponentEndOverlap.AddDynamic(this, &APSSummonBase::OnOverlapEnd);
}

void APSSummonBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!IsValid(Owner)) return;
    
	switch (SummonBehavior)
	{
	case ESummonBehavior::Stationary:
		// 아무것도 안 함 (위치 고정)
		break;
		
	case ESummonBehavior::AttachOwner:
		// 아무것도 안 함 (위치 고정)
		break;
        
	case ESummonBehavior::ChasePlayer:
		{
			const FVector ToOwner = Owner->GetActorLocation() - GetActorLocation();
			const float Distance = ToOwner.Size();
        
			const float StopDistance = 30.f;  // 이 거리 이하면 안 움직임
			if (Distance > StopDistance)
			{
				const FVector Direction = ToOwner.GetSafeNormal();
				const float MoveDelta = FMath::Min(MoveSpeed * DeltaSeconds, Distance);
				AddActorWorldOffset(Direction * MoveDelta);
			}
		}
		break;
	}
}

void APSSummonBase::Initialize(AActor* InOwner, ESummonBehavior InSummonBehavior, ESummonDamageMode InSummonDamageMode,
                               FDamageResult InDamage, float InAreaMultiplier, float InHitboxDelay, float InDuration, float InKnockBackMultiplier)
{
	Owner = InOwner;
	Tags.Empty();
	if (InOwner)
	{
		if (InOwner->ActorHasTag("Player")) Tags.Add("Player");
		else if (InOwner->ActorHasTag("Enemy")) Tags.Add("Enemy");
	}
	
	FinalDamage = InDamage;
	CollisionBox->SetBoxExtent(OriginalBoxExtent * InAreaMultiplier);
	CurrentBoxExtent = CollisionBox->GetUnscaledBoxExtent();
	KnockBackMultiplier = InKnockBackMultiplier;
	SummonBehavior = InSummonBehavior;
    SummonDamageMode = InSummonDamageMode;
	
	OverlapActors.Empty();

	if(SummonBehavior==ESummonBehavior::AttachOwner)
	{
		FAttachmentTransformRules Rules(
			EAttachmentRule::SnapToTarget,	// Location: Owner와 같이
			EAttachmentRule::KeepWorld,		// Rotation: 유지
			EAttachmentRule::KeepWorld,		// Scale: 유지
			true
		);
		AttachToActor(Owner, Rules);
	}
	
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(DurationTimerHandle);
	World->GetTimerManager().ClearTimer(HitboxDelayTimerHandle);

	switch (SummonDamageMode)
	{
	case ESummonDamageMode::OneShot:
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PerformAreaDamage();
    
		World->GetTimerManager().SetTimer(
			DurationTimerHandle, this, &APSSummonBase::ReturnToPool, OneShotDuration, false);
    
		FlipbookComponent->SetLooping(false);
		FlipbookComponent->PlayFromStart();
		break;
    
	case ESummonDamageMode::Periodic:
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PerformAreaDamage();
    
		if (InDuration > 0.f)
		{
			World->GetTimerManager().SetTimer(
				DurationTimerHandle, this, &APSSummonBase::ReturnToPool, InDuration, false);
		}

		if(InHitboxDelay > 0.f)
		{
			World->GetTimerManager().SetTimer(
				HitboxDelayTimerHandle, this, &APSSummonBase::PerformAreaDamage, InHitboxDelay, true);			
		}
    
		FlipbookComponent->SetLooping(true);
		FlipbookComponent->PlayFromStart();
		break;
	}
	
	switch (SummonBehavior)
	{
	case ESummonBehavior::Stationary:
		SetActorTickEnabled(false);
		break;
	case ESummonBehavior::AttachOwner:
		SetActorTickEnabled(false);
		break;
	case ESummonBehavior::ChasePlayer:
		SetActorTickEnabled(true);
		break;
	}
	
    bInitialized = true; 
}

void APSSummonBase::UpdateStats(FDamageResult InDamage, float InAreaMultiplier, float InHitboxDelay, float InDuration,
	float InKnockBackMultiplier)
{
	FinalDamage = InDamage;
	CollisionBox->SetBoxExtent(OriginalBoxExtent * InAreaMultiplier);
	CurrentBoxExtent = CollisionBox->GetUnscaledBoxExtent();
	KnockBackMultiplier = InKnockBackMultiplier;

	
	if (SummonDamageMode != ESummonDamageMode::Periodic) return;
	
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(DurationTimerHandle);
	World->GetTimerManager().ClearTimer(HitboxDelayTimerHandle);
	
	if (InDuration > 0.f)
	{
		World->GetTimerManager().SetTimer(
			DurationTimerHandle, this, &APSSummonBase::ReturnToPool, InDuration, false);
	}
	
	if(InHitboxDelay > 0.f)
	{
		World->GetTimerManager().SetTimer(
			HitboxDelayTimerHandle, this, &APSSummonBase::PerformAreaDamage, InHitboxDelay, true);			
	}
}

void APSSummonBase::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool_Implementation();
}

void APSSummonBase::OnReturnToPool_Implementation()
{
	Super::OnReturnToPool_Implementation();
	OverlapActors.Empty();
    AreaActors.Empty();
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
		World->GetTimerManager().ClearTimer(HitboxDelayTimerHandle);
	}
	
	// Attach 되어있으면 Detach
	if (GetAttachParentActor())
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
    bInitialized = false; 
}

void APSSummonBase::BeginPlay()
{
	Super::BeginPlay();
	OriginalBoxExtent = CollisionBox->GetUnscaledBoxExtent();
}

void APSSummonBase::PerformAreaDamage()
{
	AreaActors.Empty();
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Owner);
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CurrentBoxExtent);
	
	// 박스의 실제 월드 위치/회전 가져오기
	const FVector BoxLocation = CollisionBox->GetComponentLocation();
	const FQuat BoxRotation = CollisionBox->GetComponentQuat();
	
	TArray<FOverlapResult> SweepActors;
	GetWorld()->OverlapMultiByObjectType(
		SweepActors, BoxLocation, BoxRotation,
		ObjectParams, CollisionShape, QueryParams);

	DrawDebugBox(
		GetWorld(),
		BoxLocation,            
		CurrentBoxExtent,
		BoxRotation,            
		FColor::Red,
		false,
		0.2f,
		0,
		2.f
	);
	
	for (auto& Overlap : SweepActors)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor) continue;
		if (AreaActors.Contains(Actor)) continue;
        
		AreaActors.Add(Actor);
		UPSLibrary::ApplyDamage(Actor, FinalDamage, KnockBackMultiplier, this);
	}
}

void APSSummonBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bInitialized) return;
	if (!OtherActor || OtherActor == Owner) return;
	if(!UPSLibrary::IsHostileTo(this, OtherActor)) return;

	if (APSProjectileBase* OtherProjectile = Cast<APSProjectileBase>(OtherActor); 
		OtherProjectile && OtherProjectile->ActorHasTag("Enemy") && ActorHasTag("Player") )
	{
		UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
		Pool->ReturnToPool(OtherProjectile);
		return;
	}
	
	if (!OtherActor->IsA<APSCharacterBase>()) return;
	if(!UPSLibrary::IsAlive(OtherActor)) return;
	
	// 중복 히트 방지
	if (OverlapActors.Contains(OtherActor)) return;
	OverlapActors.Add(OtherActor);
	
	UPSLibrary::ApplyDamage(OtherActor, FinalDamage, KnockBackMultiplier, this);
}

void APSSummonBase::ReturnToPool()
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
}
