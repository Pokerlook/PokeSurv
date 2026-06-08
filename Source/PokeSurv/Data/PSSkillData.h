// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PSSkillData.generated.h"

class APSPoolableActor;
class USoundBase;

USTRUCT(BlueprintType)
struct FSkillLevelData
{
	GENERATED_BODY()
	
	// 쿨타임. -1로 설정 시 1회만 발동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "-1: 한 번만 발동 (영구 유지 스킬용)"))
	float Cooldown = 0.f;	

	// 연사간격
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float FireInterval = 0.f;	

	// 히트박스딜레이 = 적 하나
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float HitboxDelay = 0.f;	

	// 피해량
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Damage = 0.f;		

	
	//공격범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float AreaMultiplier = 0.f;

	//투사체 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ProjectileSpeedMultiplier = 0.f;

	// 투사체 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 ProjectileCount = 0;
	
	// 투사체 관통 횟수. 무한 관통엔 TNumericLimits<int32>::Max() 넣어주자
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 PierceCount = 0;	

	
	// 지속시간. 0 미만은 무한
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Duration = 0.f;	

	// 넉백
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Knockback = 0.f;	

	
	// 치명타 확률, 1.0 = 100%
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CritChance = 0.f;
	
	// 치명타 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CritMultiplier = 0.f;
	
	FSkillLevelData& operator+=(const FSkillLevelData& Other)
	{
		Cooldown += Other.Cooldown;
		FireInterval += Other.FireInterval;
		HitboxDelay += Other.HitboxDelay;
		Damage += Other.Damage;
		AreaMultiplier += Other.AreaMultiplier;
		ProjectileSpeedMultiplier += Other.ProjectileSpeedMultiplier;
		ProjectileCount += Other.ProjectileCount;
		PierceCount += Other.PierceCount;
		Duration += Other.Duration;
		Knockback += Other.Knockback;
		CritChance += Other.CritChance;
		CritMultiplier += Other.CritMultiplier;
		return *this;
	}
};

UCLASS()
class POKESURV_API UPSSkillData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString SkillName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APSPoolableActor> ProjectileClass;
	
	// 투사체제한
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ProjectileLimit = 50;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Rarity = 1;
	
	// 스킬 발동 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> ActivateSound;  
	
	// 발사체/소환물이 생성될 때마다 재생 (개별 발사 음, 화살 슉슉)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> FireSound;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SkillTag; // [문장_계열] 같은 거	*/
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) // 레벨별 데이터
	TArray<FSkillLevelData> LevelData; // 최대 8개

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
