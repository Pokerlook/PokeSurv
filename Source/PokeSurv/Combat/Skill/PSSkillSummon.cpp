// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/Skill/PSSkillSummon.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Combat/Summon/PSSummonBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Player/PSPlayerController.h"

void UPSSkillSummon::Activate()
{
	CurrentSpawnStackOffset = -SpawnStackSpacing;
	
	if (SummonSpawnPattern == ESummonSpawnPattern::OwnerLeftRightStack)
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			if (APSPlayerController* PC = Cast<APSPlayerController>(Character->GetController()))
			{
				bNextSpawnRight = PC->bLastMovedRight;
			}
		}
	}
	
	StartFireSequence();
}

void UPSSkillSummon::FireOnce()
{
    UE_LOG(LogTemp, Warning, TEXT("[%s FireOnce] called"), *GetName());
	
	if (!IsValid(Owner))
	{
		return;
	}
	
	const FVector SpawnLocation = GetSpawnLocation();
	const FRotator SpawnRotation = GetSpawnRotation();

	APSSummonBase* Summon = RequestSummon(SpawnLocation, SpawnRotation);
	if (IsValid(Summon))
	{
		Summon->Initialize(Owner, SummonBehavior, SummonDamageMode,
			CalculateDamage(), CalculateAreaMultiplier(), CalculateHitBoxDelay(), CalculateDuration(), GetKnockBack());
		
		if (SkillData && SkillData->FireSound)
		{
			UGameplayStatics::PlaySound2D(this, SkillData->FireSound);
		}

		ActiveSummons.AddUnique(Summon);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Summon spawn failed. SkillData: %s, ProjectileClass: %s"),
	*GetName(),
	*GetNameSafe(SkillData),
	SkillData ? *GetNameSafe(SkillData->ProjectileClass) : TEXT("SkillData null"));

	}
}

void UPSSkillSummon::OnLevelUp()
{
	for (int32 i = ActiveSummons.Num() - 1; i >= 0; i--)
	{
		if (APSSummonBase* Summon = ActiveSummons[i].Get();
			!Summon->IsHidden())
		{
			Summon->UpdateStats(
				CalculateDamage(),
				CalculateAreaMultiplier(),
				CalculateHitBoxDelay(),
				CalculateDuration(),
				GetKnockBack()
			);
		}
		else
		{
			ActiveSummons.RemoveAt(i);  // 죽은 거 정리
		}
	}
}

APSSummonBase* UPSSkillSummon::RequestSummon(FVector Location, FRotator Rotation)
{
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	check(PoolSubsystem);
    
	AActor* Actor = PoolSubsystem->SpawnFromPool(SkillData->ProjectileClass, Location, Rotation);
	return Cast<APSSummonBase>(Actor);
}

FVector UPSSkillSummon::GetSpawnLocation()
{
	if (!IsValid(Owner)) return FVector::ZeroVector;
    
	const FVector OwnerLoc = Owner->GetActorLocation();
    
	switch (SummonSpawnPattern)
	{
	case ESummonSpawnPattern::OwnerCenter:
		return OwnerLoc;
        
	case ESummonSpawnPattern::OwnerLeftRightStack:
		{
			const FVector ForwardVec = Owner->GetActorForwardVector();
			const FVector SpawnLoc = OwnerLoc + ForwardVec * CurrentSpawnStackOffset;
			CurrentSpawnStackOffset += SpawnStackSpacing;
			return SpawnLoc;
		}
		
	case ESummonSpawnPattern::RandomAllEnemy:
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Owner);
        
			FCollisionObjectQueryParams ObjectParams;
			ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
        
			TArray<FOverlapResult> Overlaps;
			const float SearchRadius = 2000.f;
			const FCollisionShape Sphere = FCollisionShape::MakeSphere(SearchRadius);
        
			GetWorld()->OverlapMultiByObjectType(
				Overlaps, OwnerLoc, FQuat::Identity,
				ObjectParams, Sphere, QueryParams);
        
			// "Enemy" 태그 있는 적만 필터
			TArray<AActor*> Enemies;
			for (auto& Overlap : Overlaps)
			{
				if (AActor* A = Overlap.GetActor())
				{
					if (A->ActorHasTag("Enemy"))  // 너 태그 시스템 활용
					{
						Enemies.Add(A);
					}
				}
			}
        
			if (Enemies.Num() > 0)
			{
				const int32 RandomIdx = FMath::RandRange(0, Enemies.Num() - 1);
				return Enemies[RandomIdx]->GetActorLocation();
			}
			return OwnerLoc;
		}
        
	default:
		return OwnerLoc;
	}
}

FRotator UPSSkillSummon::GetSpawnRotation()
{
	if (!IsValid(Owner)) return FRotator::ZeroRotator;
    
    
	switch (SummonSpawnPattern)
	{
	case ESummonSpawnPattern::OwnerCenter:
		return FRotator::ZeroRotator;
        
	case ESummonSpawnPattern::OwnerLeftRightStack:
		{

			const FRotator OwnerRotation = Owner->GetActorRotation();
    
			// Yaw를 기준으로 ±90도 회전
			const FRotator RightRotation = OwnerRotation + FRotator(0.f, 90.f, 0.f);
			const FRotator LeftRotation  = OwnerRotation + FRotator(0.f, -90.f, 0.f);
    
			if (bNextSpawnRight)
			{
				bNextSpawnRight = !bNextSpawnRight;
				return RightRotation;
			}
			else
			{
				bNextSpawnRight = !bNextSpawnRight;
				return LeftRotation;
			}

		}
        
	case ESummonSpawnPattern::RandomAllEnemy:
		{
			// 모든 적 중 랜덤 1마리 위치
			// PSSkillBase에 GetRandomEnemy 같은 함수가 있으면 이상적
			// 없으면 여기서 직접 구현
            
			// 일단 GetNearestEnemyDirection을 활용하거나 직접 OverlapMulti
			AActor* RandomEnemy = nullptr;  // TODO: 구현
			if (RandomEnemy)
			{
				return FRotator::ZeroRotator;
			}
			return FRotator::ZeroRotator;  // 적 없으면 자기 위치 (fallback)
		}
        
	default:
		return FRotator::ZeroRotator;
	}
}
