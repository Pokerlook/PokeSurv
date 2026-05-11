// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/PSTypes.h"
#include "PokeSurv/Data/PSSkillData.h"
#include "PSSkillBase.generated.h"

class UPSPlayerStatComponent;

UCLASS(BlueprintType, Blueprintable)
class POKESURV_API UPSSkillBase : public UObject
{
	GENERATED_BODY()

public:
	// SkillComponent가 매 틱 호출
	void TickCooldown(float DeltaTime);  

	// SkillComp->AddSkill 에서 호출할 듯 
	void InitSkill(AActor* InOwner);

	// SkillLevelUp, if check, CurrentLevel++ 
	void LevelUp();

	UFUNCTION(BlueprintCallable)
	void DebugSkill();
	

protected:
	
	virtual void Activate() PURE_VIRTUAL(UPSSkillBase::Activate,);
	
	FDamageResult CalculateDamage() const;
	float CalculateAreaMultiplayer() const;
	float CalculateProjectileSpeedMultiplayer() const;
	float CalculateDuration() const;
	float CalculateCooldown() const;
	int32 CalculateProjectileCount() const;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS")
	int32 SkillLevel = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS")
	TObjectPtr<UPSSkillData> SkillData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS")
	FSkillLevelData CurrentStats ; // 현재 누적 스탯

protected:	
	// 쿨타임 타이머
	float CooldownTimer = 0.f;
	
	// 오너 (투사체 스폰, 플레이어 위치 등에 필요)
	UPROPERTY()
	TObjectPtr<AActor> Owner;

	UPROPERTY()
	UPSPlayerStatComponent* PlayerStatComp;
};
