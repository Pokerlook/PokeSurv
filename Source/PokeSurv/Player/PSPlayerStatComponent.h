// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSPlayerStatComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POKESURV_API UPSPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Player")
	int32 PlayerLevel = 1;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	int32 MaxHPBonus = 0;            // 최대체력
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	float Recovery = 0.f;            // 초당 회복량

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	int32 Defense = 0;            // 방어력
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	float SpeedBonus = 0.f;            // 이동 속도 
	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float DamageBonus = 0.f;      // 피해량 증가
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float ProjectileSpeedBonus = 0.f;      // 발사체 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float DurationBonus = 0.f;             // 스킬 지속시간 증가율

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float AreaBonus = 0.f;			// 공격 범위
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float CooldownDecrease = 0.f;             // 쿨타임 감소 %

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	int32 ProjectileCount = 0;        // 발사체 추가수
	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	int32 Revive = 0;                 // 부활 횟수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float MagnetBonus = 0.f;              // 아이템 흡수 반경 배율

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Luck = 0.f;                // 행운

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Growth = 0.f;              // 경험치

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Greed = 0.f;               // 골드

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Curse = 0.f;               // 저주

	
	// 상점 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Shop")
	int32 Reroll = 0;                // 새로고침 횟수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Shop")
	int32 Skip = 0;                  // 건너뛰기 횟수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Shop")
	int32 Banish = 0;                // 지우기 횟수
};
