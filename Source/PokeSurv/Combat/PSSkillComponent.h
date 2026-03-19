// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSSkillComponent.generated.h"

class UPSSkillBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POKESURV_API UPSSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPSSkillComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void AddSkill(TSubclassOf<UPSSkillBase> SkillClass);// 스킬 추가되면 자동 발동 시작
	
	// LevelUpSkill
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills")
	TSubclassOf<UPSSkillBase> DefaultSkill;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skills")
	TArray<TObjectPtr<UPSSkillBase>> Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	float EnemyScanRadius = 1500.f;
	
private:
	
};
