// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Pawn/PSEnemyPawn.h"

#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Character/PSStatComponent.h"
#include "PokeSurv/Component/PSCrowdMovementComponent.h"
#include "PokeSurv/Game/PSGameModeBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"
#include "PokeSurv/Util/PSLibrary.h"

// Sets default values
APSEnemyPawn::APSEnemyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 캡슐: 물리 Block 없음, 쿼리 Overlap만 (스킬이 적을 맞혀야 하므로)
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(34.f, 34.f);
	Capsule->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Capsule->SetCanEverAffectNavigation(false);
	RootComponent = Capsule;

	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(Capsule);
	Sprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sprite->SetGenerateOverlapEvents(false);
	Sprite->SetCastShadow(false);

	CrowdMovement = CreateDefaultSubobject<UPSCrowdMovementComponent>(TEXT("CrowdMovement"));

	StatComponent = CreateDefaultSubobject<UPSStatComponent>(TEXT("StatComponent"));

	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;
}

void APSEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	StatComponent->OnStatsChanged.AddDynamic(this, &APSEnemyPawn::OnStatsChanged);
	OnStatsChanged();

	// 일반 적은 추적 (Swarm은 자식에서 덮어씀)
	CrowdMovement->MoveType = ECrowdMoveType::Chase;
}

void APSEnemyPawn::PS_TakeDamage_Implementation(const FDamageResult Damage)
{
	if (StatComponent->CurrentHP <= 0) return;

	const float FinalDamage = FMath::Max(1, (Damage.FinalDamage - StatComponent->Defense));

	if (APSPlayerController* PC = Cast<APSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->ShowDamageNumber(Damage, this);
	}

	StatComponent->CurrentHP = FMath::Max(0, StatComponent->CurrentHP - FinalDamage);

	UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());

	if (StatComponent->CurrentHP <= 0)
	{
		OnDeath();
	}
	else
	{
		OnDamage();
	}
}

void APSEnemyPawn::TakeKnockback_Implementation(float KnockBackMultiplier, FVector HitLocation)
{
	const FVector Dir = (GetActorLocation() - HitLocation).GetSafeNormal();
	const float RandomKnockback = FMath::FRandRange(
		StatComponent->KnockbackMultiplier.X, StatComponent->KnockbackMultiplier.Y);

	CrowdMovement->AddKnockback(Dir * KnockBackMultiplier * RandomKnockback * 100.f);
}

void APSEnemyPawn::OnSpawnFromPool_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	CrowdMovement->ResetMotion(); // 넉백/속도 잔여 제거
	CrowdMovement->OnSpawnFromPool();
	
	// 접촉 데미지 타이머 시작 (임시 — 나중에 PSCrowdSubsystem으로 이사)
	GetWorldTimerManager().SetTimer(ContactDamageTimer, this,
		&APSEnemyPawn::ContactDamageTick, ContactDamageInterval, true);
}

void APSEnemyPawn::OnReturnToPool_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	CrowdMovement->OnReturnFromPool();
	
	GetWorldTimerManager().ClearTimer(ContactDamageTimer);
}

bool APSEnemyPawn::ShouldCull_Implementation()
{
	return false;
}

void APSEnemyPawn::OnDamage_Implementation()
{
	// 이펙트. 하얗게 한번 깜빡이기
}

void APSEnemyPawn::OnDeath_Implementation()
{
	CrowdMovement->MaxSpeed = 0;
	SetActorEnableCollision(false);

	GetWorldTimerManager().ClearTimer(ContactDamageTimer);
	
	APSGameModeBase* GM = Cast<APSGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GM) return;
	GM->AddKillCount();
	
	HandleDrops();
}

void APSEnemyPawn::HandleDrops()
{
	if (APSGameModeBase* GM = Cast<APSGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->SpawnExpGem(GetActorLocation(), StatComponent->BaseData->XP);
	}
}

void APSEnemyPawn::RequestReturnToPool()
{
	if (UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>())
	{
		Pool->ReturnToPool(this);
	}
}

void APSEnemyPawn::ContactDamageTick()
{
	if(!IsActorTickEnabled()) return;
	// 임시: 거리 기반 접촉 데미지. OnHit(물리 Block)이 crowd엔 없어서 타이머로.
	// TODO: PSCrowdSubsystem이 Hero 거리를 이미 아니까, 나중에 그쪽으로 이사
	const float TraceRadius = Capsule->GetScaledCapsuleRadius() * 1.1f;

	TArray<FOverlapResult> Overlaps;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(),
		FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), Sphere, QueryParams);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (Actor && Actor->ActorHasTag("Player"))
		{
			FDamageResult DamageResult{ (float)StatComponent->Attack, false };
			UPSLibrary::ApplyDamage(Actor, DamageResult, 0.f, this);
			return;
		}
	}
}

void APSEnemyPawn::OnStatsChanged()
{
	CrowdMovement->MaxSpeed = StatComponent->Speed;
	OnStatsInitializedDelegate.Broadcast(); // AnimBP에서 sprite 변경
}
