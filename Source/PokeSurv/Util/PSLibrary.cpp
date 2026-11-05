// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Util/PSLibrary.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PokeSurv/Component/PSStatComponent.h"
#include "PokeSurv/Combat/PSSkillComponent.h"
#include "PokeSurv/Combat/Item/PSItemBase.h"
#include "PokeSurv/Combat/Skill/PSSkillBase.h"
#include "PokeSurv/Interface/PSDamageable.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"

DEFINE_LOG_CATEGORY(LogPSHit);

static TAutoConsoleVariable<int32> CVarPSHitLog(TEXT("ps.HitLog"), 0, TEXT("1: 발사체/소환물 오버랩 디버그 로그"));

bool PSHitDebug::IsOn()
{
	return CVarPSHitLog.GetValueOnGameThread() != 0;
}

FString PSHitDebug::DescribeCol(const UPrimitiveComponent* C, const UPrimitiveComponent* Other)
{
	if (!C) return TEXT("null");
	const ECollisionChannel OtherType = Other ? Other->GetCollisionObjectType() : ECC_Pawn;
	return FString::Printf(TEXT("%s[%s] Obj=%s En=%d GenOv=%d Resp->Other=%d"),
		*GetNameSafe(C->GetOwner()), *C->GetName(),
		*UEnum::GetValueAsString(C->GetCollisionObjectType()),
		(int32)C->GetCollisionEnabled(), C->GetGenerateOverlapEvents(),
		(int32)C->GetCollisionResponseToChannel(OtherType));   // 0 Ignore / 1 Overlap / 2 Block
}

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
		UE_LOG(LogTemp, Warning, TEXT("[IsAlive] Actor is invalid (null or pending kill)"));
		return false;
	}

	const UPSStatComponent* Stat = Actor->FindComponentByClass<UPSStatComponent>();
	if (!Stat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[IsAlive] %s has no PSStatComponent"), *Actor->GetName());
		return false;
	}
    
	return Stat->CurrentHP > 0.f;
}

int32 UPSLibrary::GetSkillRarity(const TSubclassOf<UPSSkillBase>& SkillClass)
{
    return SkillClass.GetDefaultObject()->SkillData->Rarity;
}

int32 UPSLibrary::GetItemRarity(const TSubclassOf<UPSItemBase>& ItemClass)
{
    return ItemClass.GetDefaultObject()->ItemData->Rarity;
}

FString UPSLibrary::GetOptionName(const FPSLevelUpOption& Option)
{
	return Option.bIsSkill
		? Option.SkillClass.GetDefaultObject()->SkillData->SkillName
		: Option.ItemClass.GetDefaultObject()->ItemData->ItemName;
}

UTexture2D* UPSLibrary::GetOptionIcon(const FPSLevelUpOption& Option)
{
	return Option.bIsSkill
		? Option.SkillClass.GetDefaultObject()->SkillData->Icon
		: Option.ItemClass.GetDefaultObject()->ItemData->Icon;
}

FText UPSLibrary::GetOptionBaseDescription(const FPSLevelUpOption& Option)
{
	return Option.bIsSkill
		? Option.SkillClass.GetDefaultObject()->SkillData->Description
		: Option.ItemClass.GetDefaultObject()->ItemData->Description;
}

UPSSkillComponent* UPSLibrary::GetPlayerSkillComponent(const UObject* WorldContextObject)
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0);
	return Player ? Player->FindComponentByClass<UPSSkillComponent>() : nullptr;
}