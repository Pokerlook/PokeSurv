// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PokeSurv/PSTypes.h"
#include "PSSkillComponent.generated.h"

class UPSSkillBase;
class UPSItemBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POKESURV_API UPSSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPSSkillComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 스킬 추가 or 레벨업
	UFUNCTION(BlueprintCallable)
	void AddSkill(TSubclassOf<UPSSkillBase> SkillClass);// 스킬 추가되면 자동 발동 시작
	
	// 아이템 추가 or 레벨업
	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<UPSItemBase> ItemClass);
	
	UPSSkillBase* FindSkill(const TSubclassOf<UPSSkillBase>& SkillClass) const;
	UPSItemBase* FindItem(const TSubclassOf<UPSItemBase>& ItemClass) const;
	
	UFUNCTION(BlueprintPure, Category = "PS")
	bool IsOptionNew(const FPSLevelUpOption& Option) const;

	// 보유 중이면 현재 레벨, 미보유면 0 (또는 INDEX_NONE)
	UFUNCTION(BlueprintPure, Category = "PS")
	int32 GetOptionCurrentLevel(const FPSLevelUpOption& Option) const;
	
	UFUNCTION(BlueprintPure, Category = "PS")
	FText GetOptionNextLevelDescription(const FPSLevelUpOption& Option) const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PS|Skills")
	TArray<TObjectPtr<UPSSkillBase>> Skills;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PS|Items")
	TArray<TObjectPtr<UPSItemBase>> Items;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PS")
	int32 MaxSlotPerType = 6;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PS")
	float EnemyScanRadius = 1500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PS")
	TSubclassOf<UPSSkillBase> DefaultSkill;
};
