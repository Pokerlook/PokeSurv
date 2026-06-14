// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSPlayerStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStatChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExpChanged, float, CurrentExp, float, MaxExp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int, Gold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int, Level);

UENUM(BlueprintType)
enum class EPlayerStatType : uint8
{
	MaxHPBonus          UMETA(DisplayName = "MaxHPBonus",           ToolTip = "최대체력 보너스"),
	Recovery            UMETA(DisplayName = "Recovery",             ToolTip = "초당 회복량"),
	Defense             UMETA(DisplayName = "Defense",              ToolTip = "방어력"),
	SpeedBonus          UMETA(DisplayName = "SpeedBonus",           ToolTip = "이동 속도"),

	DamageBonus         UMETA(DisplayName = "DamageBonus",          ToolTip = "피해량 증가"),
	ProjectileSpeedBonus UMETA(DisplayName = "ProjectileSpeedBonus", ToolTip = "발사체 속도"),
	DurationBonus       UMETA(DisplayName = "DurationBonus",        ToolTip = "스킬 지속시간"),
	AreaBonus           UMETA(DisplayName = "AreaBonus",            ToolTip = "공격 범위"),
	CooldownDecrease    UMETA(DisplayName = "CooldownDecrease",     ToolTip = "쿨타임 감소(음수)"),
	ProjectileCount     UMETA(DisplayName = "ProjectileCount",      ToolTip = "발사체 추가수"),

	Revive              UMETA(DisplayName = "Revive",               ToolTip = "부활 횟수"),
	MagnetBonus         UMETA(DisplayName = "MagnetBonus",          ToolTip = "아이템 흡수 반경"),
	Luck                UMETA(DisplayName = "Luck",                 ToolTip = "행운"),
	Growth              UMETA(DisplayName = "Growth",               ToolTip = "경험치 보너스"),
	Greed               UMETA(DisplayName = "Greed",                ToolTip = "골드 보너스"),
	Curse               UMETA(DisplayName = "Curse",                ToolTip = "저주"),

	Reroll              UMETA(DisplayName = "Reroll",               ToolTip = "새로고침 횟수"),
	Skip                UMETA(DisplayName = "Skip",                 ToolTip = "건너뛰기 횟수"),
	Banish              UMETA(DisplayName = "Banish",               ToolTip = "지우기 횟수"),
};

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POKESURV_API UPSPlayerStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
	
	// 스탯 추가 (아이템 획득, 레벨업 등)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddStat(EPlayerStatType StatType, float Value);

	// 스탯 조회 (float으로 통일, int는 캐스팅)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stats")
	float GetStat(EPlayerStatType StatType) const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddExp(float Value);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddGold(float Value);

	void DebugPlayerStat();
	
private:
	int32 GetRequiredExp() const;

	void LevelUp();

public:
	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnPlayerStatChanged OnPlayerStatChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnExpChanged OnExpChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnGoldChanged OnGoldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Stats")
	FOnLevelUp OnLevelUp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Player")
	int32 PlayerLevel = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Player")
	float Exp = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Player")
	int32 Gold = 0;

	
	//// Character
	// 최대체력 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	int32 MaxHPBonus = 0;
	
	// 초당 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	float Recovery = 0.f;
	
	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	int32 Defense = 0;            

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Character")
	float SpeedBonus = 0.f;             
	

	//// Skill
	// 피해량 증가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float DamageBonus = 0.f;      

	// 발사체 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float ProjectileSpeedBonus = 0.f;      

	// 스킬 지속시간 증가율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float DurationBonus = 0.f;             

	// 공격 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float AreaBonus = 0.f;			

	// 쿨타임 감소 %
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	float CooldownDecrease = 0.f;             

	// 발사체 추가수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Skill")
	int32 ProjectileCount = 0;        
	

	//// Meta
	// 부활 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	int32 Revive = 0;                 

	// 아이템 흡수 반경 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float MagnetBonus = 0.f;         

	// 행운
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Luck = 0.f;                

	// 경험치 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Growth = 0.f;              

	// 골드 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Greed = 0.f;               

	// 저주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Meta")
	float Curse = 0.f;               


	////Shop
	// 새로고침 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Shop")
	int32 Reroll = 0;                

	// 건너뛰기 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Shop")
	int32 Skip = 0;                  

	// 지우기 횟수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Shop")
	int32 Banish = 0;                
};
