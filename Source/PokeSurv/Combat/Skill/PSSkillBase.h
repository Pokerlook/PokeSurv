// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/PSTypes.h"
#include "PokeSurv/Data/SkillData.h"
#include "PSSkillBase.generated.h"


UCLASS(BlueprintType, Blueprintable)
class POKESURV_API UPSSkillBase : public UObject
{
	GENERATED_BODY()

public:
	void TickCooldown(float DeltaTime);  // SkillComponent가 매 틱 호출

	// SkillComp->AddSkill 에서 호출할 듯 
	void InitSkill(AActor* InOwner);

	// SkillLevelUp, if check, CurrentLevel++ 

protected:
	virtual void Activate() PURE_VIRTUAL(UPSSkillBase::Activate,);
	FDamageResult CalculateDamage() const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkillData> SkillData;
	
	UPROPERTY(BlueprintReadOnly)
	FSkillLevelData CurrentStats ; // 현재 누적 스탯
	
	// 현재 레벨
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLevel = 0;  // LevelData[0]이 1레벨

protected:	
	// 쿨타임 타이머
	float CooldownTimer = 0.f;
	
	// 오너 (투사체 스폰, 플레이어 위치 등에 필요)
	UPROPERTY()
	TObjectPtr<AActor> Owner;
};
