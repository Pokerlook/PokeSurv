// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PokeSurv/Util/CrowdRules.h"
#include "PSCrowdMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class POKESURV_API UPSCrowdMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UPSCrowdMovementComponent();
	
	/** 외부 충격(넉백). 의도이동과 별개로 감쇠하며 위치에 더해짐 */
	void AddKnockback(const FVector& WorldImpulse);

	/** 풀 재사용 대비: 속도/넉백 잔여 제거 */
	void ResetMotion();

	void OnSpawnFromPool();
	void OnReturnFromPool();
	
protected:
	virtual void BeginPlay() override;
//	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void UpdateFacing(const FVector2f& Vel);

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	ECrowdMoveType MoveType = ECrowdMoveType::Chase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	float MaxSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	float Acceleration = 1500.f;
	
	/** Chase: true면 A를 향해 직접 이동. false면 AddMovementInput */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Chase")
	bool bChaseHero = true;
	
	/** Flow: 흐름 방향 (평면 밖 성분 무시) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Flow")
	FVector FlowDirection = FVector::ForwardVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd")
	FCrowdTuning Tuning;
	
	/** 이 속도(평면 X축 성분) 이상일 때만 방향 전환. 밀림 떨림 방지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Sprite")
	float FacingChangeThreshold = 20.f;

	/** 마지막으로 의미 있게 움직인 방향 (평면 2D, 정규화). PaperZD Directionality 입력용 */
	UPROPERTY(BlueprintReadOnly, Category = "Crowd|Sprite")
	FVector2D FacingDirection = FVector2D(1.0, 0.0);
	
	/** 넉백 감쇠. 1초 뒤 남는 비율 (작을수록 빨리 멈춤) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Knockback")
	float KnockbackDamping = 0.0001f;
	
	UPROPERTY(EditAnywhere, Category = "Crowd|Debug")
	bool bDrawDebug = false;

	/** Chase: 이번 프레임 A에게 막혔는지. AnimBP에서 사용 */
	UPROPERTY(BlueprintReadOnly, Category = "Crowd|Chase")
	bool bBlockedByHero = false;

	/** Chase: 연속으로 막혀 있던 시간 */
	UPROPERTY(BlueprintReadOnly, Category = "Crowd|Chase")
	float BlockedTime = 0.f;
	
	float GetCrowdRadius() const { return Radius; }
	virtual float GetMaxSpeed() const override { return MaxSpeed; }
	
private:
	float Radius = 35.f;

	FVector2f KnockbackVel = FVector2f::ZeroVector;
};
