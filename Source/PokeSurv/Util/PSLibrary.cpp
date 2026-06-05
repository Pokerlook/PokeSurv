// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Util/PSLibrary.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PokeSurv/Character/PSCharacterBase.h"
#include "PokeSurv/Character/PSStatComponent.h"
#include "PokeSurv/Combat/PSSkillComponent.h"
#include "PokeSurv/Combat/Item/PSItemBase.h"
#include "PokeSurv/Combat/Skill/PSSkillBase.h"
#include "PokeSurv/Interface/PSDamageable.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"

void UPSLibrary::ApplyDamage(AActor* Target, FDamageResult Damage, float KnockBackMultiplier, AActor* DamageCauser)
{
	if (!Target || !DamageCauser) return;

	// 적대 관계 아니면 무시
	if (!IsHostileTo(DamageCauser, Target)) return;

	if (Target->Implements<UPSDamageable>())
	{
		IPSDamageable::Execute_PS_TakeDamage(Target, Damage);
		if (KnockBackMultiplier > 0.f)
		{
			IPSDamageable::Execute_TakeKnockback(Target, KnockBackMultiplier, DamageCauser->GetActorLocation());
		}
	}
}

bool UPSLibrary::IsHostileTo(AActor* A, AActor* B)
{
	if (!A || !B) return false;
    
	const bool bAIsPlayer = A->ActorHasTag("Player");
	const bool bAIsEnemy  = A->ActorHasTag("Enemy");
	const bool bBIsPlayer = B->ActorHasTag("Player");
	const bool bBIsEnemy  = B->ActorHasTag("Enemy");
    
	// Player vs Enemy
	if (bAIsPlayer && bBIsEnemy) return true;
	if (bAIsEnemy && bBIsPlayer) return true;
    
	// 그 외(같은 팀, 태그 없음): 적대 아님
	return false;
}

FText UPSLibrary::FormatTime_MS(int32 Seconds)
{
	int32 Minutes = Seconds / 60;
	int32 RemSeconds = Seconds % 60;

	FText TimeText = FText::FromString(
		FString::Printf(TEXT("%02d : %02d"), Minutes, RemSeconds)
	);
	return TimeText;
}

int32 UPSLibrary::GetElapsedSeconds(const UObject* WorldContextObject)
{
	int32 time =  UKismetMathLibrary::FTrunc(UKismetSystemLibrary::GetGameTimeInSeconds(WorldContextObject));
	return time;
}

void UPSLibrary::DebugPlayerSkillsAndItems(const UObject* WorldContextObject)
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);
	UPSSkillComponent* SkillComponent = Character->FindComponentByClass<UPSSkillComponent>();
	
	for (UPSSkillBase* Skill : SkillComponent->Skills)
	{
		if (!Skill) continue;
		UE_LOG(LogTemp, Warning, TEXT("[Skill] %s LV%d"), *Skill->GetName(), Skill->SkillLevel);
		Skill->DebugSkill();
	}
	for (UPSItemBase* Item : SkillComponent->Items)
	{
		if (!Item) continue;
		UE_LOG(LogTemp, Warning, TEXT("[Item] %s LV%d"), *Item->GetName(), Item->ItemLevel);              
	}

}

void UPSLibrary::DebugPlayerStat(const UObject* WorldContextObject)
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);
	UPSPlayerStatComponent* PlayerStatComponent = Character->FindComponentByClass<UPSPlayerStatComponent>();

	PlayerStatComponent->DebugPlayerStat();
}

bool UPSLibrary::IsAlive(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[IsAlive] Actor is invalid (null or pending kill)"));
		return false;
	}
    
	APSCharacterBase* Character = Cast<APSCharacterBase>(Actor);
	if (!Character)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[IsAlive] %s is not APSCharacterBase"), *Actor->GetName());
		return false;
	}
    
	UPSStatComponent* Stat = Character->FindComponentByClass<UPSStatComponent>();
	if (!Stat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[IsAlive] %s has no PSStatComponent"), *Actor->GetName());
		return false;
	}
    
	return Stat->CurrentHP > 0.f;
}
