// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Pawn/PSEnemyPawn.h"

#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Component/PSStatComponent.h"
#include "PokeSurv/Component/PSCrowdMovementComponent.h"
#include "PokeSurv/Game/PSGameModeBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"
#include "PokeSurv/Util/PSLibrary.h"
#include "PaperZDAnimationComponent.h"

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
	
	AnimationComponent = CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("Animation"));
	AnimationComponent->InitRenderComponent(Sprite);

	CrowdMovement = CreateDefaultSubobject<UPSCrowdMovementComponent>(TEXT("CrowdMovement"));

	StatComponent = CreateDefaultSubobject<UPSStatComponent>(TEXT("StatComponent"));

	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;
	
    Tags.AddUnique(TEXT("Enemy"));
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
	ActivateEnemy();
}

void APSEnemyPawn::OnReturnToPool_Implementation()
{
	DeactivateEnemy();
}

bool APSEnemyPawn::ShouldCull_Implementation()
{
	return false;
}

TSubclassOf<UPaperZDAnimInstance> APSEnemyPawn::GetSequencerAnimInstanceClass() const
{
	return AnimationComponent->GetSequencerAnimInstanceClass();
}

UPaperZDAnimInstance* APSEnemyPawn::GetSequencerAnimInstance()
{
	return AnimationComponent->GetSequencerAnimInstance();
}

FVector2D APSEnemyPawn::GetFacingDirection() const
{
	// crowd는 월드 XY 기준 → PaperZD는 화면 기준 (화면 가로 = 월드 Y, 화면 세로 = 월드 X)
	const FVector2D F = CrowdMovement->FacingDirection;
	return FVector2D(F.Y, F.X);
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
	// crowd 멈춤 거리와 같은 기준으로 판정 (TODO: 나중에 PSCrowdSubsystem 루프로 이사)
	if (IsHidden() || StatComponent->CurrentHP <= 0) return;
	if (!CrowdMovement->IsTouchingHero()) return;

	if (APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		FDamageResult DamageResult{ (float)StatComponent->Attack, false };
		UPSLibrary::ApplyDamage(Player, DamageResult, 0.f, this);
	}
}

void APSEnemyPawn::ActivateEnemy()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	CrowdMovement->ResetMotion();
	CrowdMovement->OnSpawnFromPool();   // crowd 등록

	Sprite->SetComponentTickEnabled(true);
	AnimationComponent->SetComponentTickEnabled(true);
	
	// 접촉 데미지 타이머 시작 (임시 — 나중에 PSCrowdSubsystem으로 이사)
	GetWorldTimerManager().SetTimer(ContactDamageTimer, this,
		&APSEnemyPawn::ContactDamageTick, ContactDamageInterval, true);
}

void APSEnemyPawn::DeactivateEnemy()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	CrowdMovement->OnReturnFromPool();  // crowd 해제
	
	Sprite->SetComponentTickEnabled(false);
	AnimationComponent->SetComponentTickEnabled(false);

	// 접촉 데미지 타이머 종료 (임시 — 나중에 PSCrowdSubsystem으로 이사)
	GetWorldTimerManager().ClearTimer(ContactDamageTimer);
}

void APSEnemyPawn::OnStatsChanged()
{
	CrowdMovement->MaxSpeed = StatComponent->Speed;
	OnStatsInitializedDelegate.Broadcast(); // AnimBP에서 sprite 변경
}
