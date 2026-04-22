// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PokeSurv/PSTypes.h"
#include "PSDamageTextWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageTextFinished, UPSDamageTextWidget*, Widget);

/**
 * 
 */
UCLASS()
class POKESURV_API UPSDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(FDamageResult DamageResult);

	// BP에서 PlayAnimation 후 Player 넘겨주면 바인딩
	UFUNCTION(BlueprintCallable)
	void BindAnimationFinished(UUMGSequencePlayer* Player);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    FVector WorldLocation; // 붙을 월드 좌표
	
	UPROPERTY(BlueprintAssignable)
	FOnDamageTextFinished OnDamageTextFinished;
};
