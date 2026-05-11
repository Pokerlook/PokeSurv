// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Items")
	TArray<TObjectPtr<UPSItemBase>> Items;
private:
	
};
