// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Data/PSItemData.h"
#include "PSItemBase.generated.h"

class UPSPlayerStatComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class POKESURV_API UPSItemBase : public UObject
{
	GENERATED_BODY()
public:
	
	void InitItem(AActor* InOwner);
	void LevelUp();
	
	bool IsMaxLevel() const
	{
		return ItemData && ItemLevel >= ItemData->MaxLevel;
	}
	
protected:
	UPROPERTY() // 필요없을지도 모름
	TObjectPtr<AActor> Owner;
	
	UPROPERTY()
	UPSPlayerStatComponent* PlayerStatComp;
	
public:
	// 이 레벨은 인덱스 서치에 사용되므로, +1을 해야 플레이어가 보는 레벨과 동일하다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemLevel = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PS")
	TObjectPtr<UPSItemData> ItemData;
};
