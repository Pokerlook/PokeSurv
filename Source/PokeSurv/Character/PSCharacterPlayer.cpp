// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Character/PSCharacterPlayer.h"

#include "Components/SphereComponent.h"
#include "PokeSurv/Combat/PSSkillComponent.h"
#include "PokeSurv/Character/PSStatComponent.h"
#include "PokeSurv/Combat/Pickup/PSPickupBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"
#include "PokeSurv/UI/PSHealthWidgetComponent.h"

APSCharacterPlayer::APSCharacterPlayer()
{
	PlayerStatComponent = CreateDefaultSubobject<UPSPlayerStatComponent>(TEXT("PlayerStatComponent"));
	SkillComponent = CreateDefaultSubobject<UPSSkillComponent>(TEXT("SkillComponent"));
	HPBarComponent = CreateDefaultSubobject<UPSHealthWidgetComponent>("HPBarComponent");
	HPBarComponent->SetupAttachment(GetRootComponent());
	
	PickupMagnet = CreateDefaultSubobject<USphereComponent>(TEXT("PickupMagnet"));
	PickupMagnet->SetupAttachment(RootComponent);
}

void APSCharacterPlayer::PS_TakeDamage_Implementation(const FDamageResult Damage)
{
	Super::PS_TakeDamage_Implementation(Damage);
	HPBarComponent->UpdateHP(StatComponent->CurrentHP, StatComponent->MaxHP);
}

void APSCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	PSPlayerController = Cast<APSPlayerController>(GetController());
	
	// InitWidget 명시적으로 보장
	HPBarComponent->InitWidget();
    
	if (UUserWidget* Widget = HPBarComponent->GetUserWidgetObject())
	{
		HPBarComponent->UpdateHP(StatComponent->CurrentHP, StatComponent->MaxHP);
	}

	BaseMagnetRadius = PickupMagnet->GetScaledSphereRadius();
	PickupMagnet->OnComponentBeginOverlap.AddDynamic(this, &APSCharacterPlayer::OnPickupOverlapBegin);

	PlayerStatComponent->OnPlayerStatChanged.AddDynamic(this, &APSCharacterPlayer::OnPlayerStatUpdated);
}

void APSCharacterPlayer::UpdateMoveVector(float DeltaSeconds)
{
	FVector Velocity = GetVelocity();
    
	if (Velocity.Size() < 0.1f) return;
    if (!PSPlayerController) return;
    
	// LastMoveDirection은 입력 시점에 이미 정규화된 방향
	// X,Y 중 더 큰 축으로 4방향 결정
	FVector Dir = PSPlayerController->LastMoveDirection;
	if (FMath::Abs(Dir.Y) >= FMath::Abs(Dir.X))
	{
		MoveVector2D.X = Dir.Y > 0 ? 1 : -1;
		MoveVector2D.Y = 0;
	}
	else
	{
		MoveVector2D.Y = Dir.X > 0 ? 1 : -1;
		MoveVector2D.X = 0;
	}
}

void APSCharacterPlayer::OnPickupOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APSPickupBase* Pickup = Cast<APSPickupBase>(OtherActor);
	if (!Pickup) return;

	Pickup->OnPickupCollected(this);
}

void APSCharacterPlayer::OnPlayerStatUpdated()
{
    StatComponent->RecalculateFromPlayerStat(PlayerStatComponent);
}
