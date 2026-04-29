// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PSSkillData.generated.h"

class APSProjectileBase;
class USoundBase;

USTRUCT(BlueprintType)
struct FSkillLevelData
{
	GENERATED_BODY()
	
	// 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Cooldown = 0.f;	

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

	
	// 지속시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Duration = 0.f;	

	// 넉백
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float Knockback = 0.f;	

	
	// 치명타 확률, 1.0 = 100%
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CritChance = 0.f;
	
	// 치명타 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CritMultiplier = 0.f;	
};

UCLASS()
class POKESURV_API UPSSkillData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString SkillName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APSProjectileBase> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ProjectileLimit = 50;	// 투사체제한
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Rarity = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> ActivateSound;  // 스킬 발동 사운드

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SkillTag; // [문장_계열] 같은 거	*/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly) // 레벨별 데이터
	TArray<FSkillLevelData> LevelData; // 최대 8개

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};
