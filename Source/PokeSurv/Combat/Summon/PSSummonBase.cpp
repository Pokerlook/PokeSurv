// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Summon/PSSummonBase.h"

#include "PaperFlipbookComponent.h"
#include "Components/BoxComponent.h"
#include "PokeSurv/Character/PSCharacterBase.h"
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
	UWorld* World = GetWorld();
	
	FinalDamage = InDamage;
	CollisionBox->SetBoxExtent(OriginalBoxExtent * InAreaMultiplier);
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
	
	World->GetTimerManager().ClearTimer(DurationTimerHandle);
	World->GetTimerManager().ClearTimer(HitboxDelayTimerHandle);
	
	if (SummonDamageMode == ESummonDamageMode::OneShot)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PerformAreaDamage();
		
		World->GetTimerManager().SetTimer(
		DurationTimerHandle,
		this,
		&APSSummonBase::ReturnToPool,
		OneShotDuration,
		false   
		);
		
		FlipbookComponent->SetLooping(false);
		FlipbookComponent->PlayFromStart();
	}
	else
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PerformAreaDamage();
		
		World->GetTimerManager().SetTimer(
		DurationTimerHandle,
		this,
		&APSSummonBase::ReturnToPool,
		InDuration,    // InDuration초 후 풀에 반환
		false    
		);

		// Period 데미지
		World->GetTimerManager().SetTimer(
			HitboxDelayTimerHandle, this, 
			&APSSummonBase::PerformAreaDamage, 
			InHitboxDelay, true);	// HitboxDelay마다 데미지
		
		FlipbookComponent->SetLooping(true);
		FlipbookComponent->PlayFromStart();
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

	UE_LOG(LogTemp, Warning, TEXT("[Summon] FlipbookComponent Scale: %s"),
	*FlipbookComponent->GetRelativeScale3D().ToString());
	UE_LOG(LogTemp, Warning, TEXT("[Summon] FlipbookComponent World Scale: %s"),
		*FlipbookComponent->GetComponentScale().ToString());
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
}

void APSSummonBase::BeginPlay()
{
	Super::BeginPlay();
	OriginalBoxExtent = CollisionBox->GetScaledBoxExtent();
}

void APSSummonBase::PerformAreaDamage()
{
	AreaActors.Empty();
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(Owner);
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionBox->GetScaledBoxExtent());
	
	TArray<FOverlapResult> SweepActors;
	GetWorld()->OverlapMultiByObjectType(
		SweepActors, GetActorLocation(), GetActorQuat(),
		ObjectParams, CollisionShape, QueryParams);
	
	for (auto& Overlap : SweepActors)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor) continue;
		if (AreaActors.Contains(Actor)) continue;
        
		AreaActors.Add(Actor);
		UPSLibrary::ApplyDamage(Actor, FinalDamage, KnockBackMultiplier, Owner);
	}
}

void APSSummonBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == Owner) return;
	if (!OtherActor->IsA<APSCharacterBase>()) return;
	if(!UPSLibrary::IsAlive(OtherActor)) return;
	
	// 중복 히트 방지
	if (OverlapActors.Contains(OtherActor)) return;
	OverlapActors.Add(OtherActor);
	
	UPSLibrary::ApplyDamage(OtherActor, FinalDamage, KnockBackMultiplier, Owner);
}

void APSSummonBase::ReturnToPool()
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
}
