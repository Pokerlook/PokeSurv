// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Component/PSStatComponent.h"

APSCharacterBase::APSCharacterBase()
{
	StatComponent = CreateDefaultSubobject<UPSStatComponent>("StatComponent");
}

void APSCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    
	MoveSpeed = GetVelocity().Size();

	UpdateMoveVector(DeltaSeconds);
}

void APSCharacterBase::PS_TakeDamage_Implementation(const FDamageResult  Damage)
{
    if (StatComponent->CurrentHP <= 0) return;	// 이미 죽음
	
	float FinalDamage = FMath::Max(1, (Damage.FinalDamage - StatComponent->Defense) );
	//UE_LOG(LogTemp, Warning, TEXT("[TakeDamage] %s | 입력 데미지: %.1f | 방어력: %d | 최종 데미지: %.1f"),
	//	*GetName(), Damage, StatComponent->Defense, FinalDamage);
	
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

void APSCharacterBase::TakeKnockback_Implementation(float KnockBackMultiplier, FVector HitLocation)
{
	FVector KnockbackDir = (GetActorLocation() - HitLocation).GetSafeNormal();
	
	//UE_LOG(LogTemp, Warning, TEXT("[TakeKnockback] KnockBackMultiplier: %f | HitLocation: %s | KnockbackDir: %s | FinalForce: %f"),
	//	KnockBackMultiplier, *HitLocation.ToString(), *KnockbackDir.ToString(),
	//	KnockBackMultiplier * StatComponent->KnockbakMultiplier * 100);
	
	float RandomKnockback = FMath::FRandRange(StatComponent->KnockbackMultiplier.X, StatComponent->KnockbackMultiplier.Y);
	LaunchCharacter(KnockbackDir * KnockBackMultiplier * RandomKnockback * 100, true, false);
}

void APSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	StatComponent->OnStatsChanged.AddDynamic(this, &APSCharacterBase::OnStatsChanged);
	OnStatsChanged();
}

void APSCharacterBase::OnDamage_Implementation()
{
	// 이펙트? + 하얗게 한번 깜빡이기
}


void APSCharacterBase::OnDeath_Implementation()
{
    //UE_LOG(LogTemp, Warning, TEXT("[OnDeath] %s 사망 처리 시작"), *GetName());
	
	GetCharacterMovement()->MaxWalkSpeed = 0;
	SetActorEnableCollision(false);
}

void APSCharacterBase::OnRevive_Implementation()
{
	
}

void APSCharacterBase::OnStatsChanged()
{
    GetCharacterMovement()->MaxWalkSpeed = StatComponent->Speed;
    OnStatsInitializedDelegate.Broadcast(); // 애님BP로 전달, sprite 변경
}
