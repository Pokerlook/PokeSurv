// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterEnemy.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Character/PSStatComponent.h"
#include "PokeSurv/Game/PSGameModeBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"
#include "PokeSurv/Util/PSLibrary.h"

void APSCharacterEnemy::PS_TakeDamage_Implementation(const FDamageResult Damage)
{
	float FinalDamage = FMath::Max(1, (Damage.FinalDamage - StatComponent->Defense) );
	//UE_LOG(LogTemp, Warning, TEXT("[TakeDamage] %s | 입력 데미지: %.1f | 방어력: %d | 최종 데미지: %.1f"),
	//	*GetName(), Damage, StatComponent->Defense, FinalDamage);

	if(APSPlayerController* PC = Cast<APSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
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

void APSCharacterEnemy::OnSpawnFromPool_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void APSCharacterEnemy::OnReturnToPool_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
}

bool APSCharacterEnemy::ShouldCull_Implementation()
{
	return false;
}

void APSCharacterEnemy::BeginPlay()
{
	Super::BeginPlay();
	TargetCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APSCharacterEnemy::OnHit);
}

void APSCharacterEnemy::UpdateMoveVector(float DeltaSeconds)
{
	FVector ToPlayer = TargetCharacter->GetActorLocation() - GetActorLocation();
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(ToPlayer.Y, ToPlayer.X));

	if      (Angle >= -45.f  && Angle < 45.f)   MoveVector2D = FVector2D(0, 1);
	else if (Angle >= 45.f   && Angle < 135.f)  MoveVector2D = FVector2D(1, 0);
	else if (Angle >= -135.f && Angle < -45.f)  MoveVector2D = FVector2D(-1, 0);
	else                                         MoveVector2D = FVector2D(0, -1);
}

void APSCharacterEnemy::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();
    GetWorldTimerManager().ClearTimer(ContactDamageTimer);
	
	APSGameModeBase* GM = Cast<APSGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GM) return;
	GM->AddKillCount();
	
    HandleDrops();
}

void APSCharacterEnemy::HandleDrops()
{
	// 경험치 보석 만들기
	if (APSGameModeBase* GM = Cast<APSGameModeBase>(GetWorld()->GetAuthGameMode()))
		GM->SpawnExpGem(GetActorLocation(), StatComponent->BaseData->XP);
}

void APSCharacterEnemy::RequestReturnToPool()
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	Pool->ReturnToPool(this);
}

void APSCharacterEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor->ActorHasTag("Player")) return;

	// 타이머 돌고 있으면 무시
	if (GetWorldTimerManager().IsTimerActive(ContactDamageTimer)) return;
	
	// 데미지
	FDamageResult DamageResult{(float)StatComponent->Attack, false};
	UPSLibrary::ApplyDamage(OtherActor, DamageResult, 0.f, this);
    
	// 타이머 초기화 후 재시작
	GetWorldTimerManager().ClearTimer(ContactDamageTimer);
	GetWorldTimerManager().SetTimer(ContactDamageTimer, this,
		&APSCharacterEnemy::ContactDamageTrace, ContactDamageInterval, false);
}

void APSCharacterEnemy::ContactDamageTrace()
{
	// 캡슐보다 살짝 크게 오버랩 체크
	float TraceRadius = GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.1f;
    
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
    
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(),
		FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), Sphere, QueryParams);

	for (auto& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (Actor && Actor->ActorHasTag("Player"))
		{
			FDamageResult DamageResult{(float)StatComponent->Attack, false};
			UPSLibrary::ApplyDamage(Actor, DamageResult, 0.f, this);
			// 타이머 재시작
			GetWorldTimerManager().SetTimer(ContactDamageTimer, this,
				&APSCharacterEnemy::ContactDamageTrace, ContactDamageInterval, false);
			return;
		}
	}
	
	// 플레이어 없으면 타이머 종료
}
