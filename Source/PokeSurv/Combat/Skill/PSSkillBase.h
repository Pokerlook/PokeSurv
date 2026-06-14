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
	
	bool IsMaxLevel() const
	{
		return SkillData && SkillLevel >= SkillData->LevelData.Num();
	}

protected:
	
	virtual void Activate();
	
	// 한 발(혹은 한 사이클) 실행 - 자식 클래스가 구현
	virtual void FireOnce() {}

	virtual void OnLevelUp() {}
	
	// 시퀀스 시작 - 자식 클래스의 Activate()에서 호출
	void StartFireSequence();
    void HandleFireTick();
	
	FDamageResult CalculateDamage() const;
	float CalculateAreaMultiplier() const;
	float CalculateDuration() const;
	float CalculateCooldown() const;
	float CalculateProjectileSpeedMultiplier() const;
	int32 CalculateProjectileCount() const;
	int32 CalculateProjectilePierceCount() const;
	float CalculateHitBoxDelay() const;

	float GetKnockBack() const;
	float GetFireInterval() const;
	
public:
	// 이 레벨은 인덱스 서치에 사용되므로, +1을 해야 플레이어가 보는 레벨과 동일하다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS")
	int32 SkillLevel = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS")
	TObjectPtr<UPSSkillData> SkillData;

protected:
	
	// 오너 (투사체 스폰, 플레이어 위치 등에 필요)
	UPROPERTY()
	TObjectPtr<AActor> Owner;

private:
    bool bActivatedOnce = false;  // Cooldown < 0일 때 1회 발동 후 종료 체크
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PS")
	FSkillLevelData CurrentStats ; // 현재 누적 스탯
	
	// 쿨타임 타이머
	float CooldownTimer = 0.f;

	UPROPERTY()
	UPSPlayerStatComponent* PlayerStatComp;
	
	// 발사 시퀀스 관리
	UPROPERTY()
	int32 RemainingFireCount = 0;

	FTimerHandle FireIntervalTimerHandle;
};
